#include "parser.h"
#include <stdlib.h>
#include <string.h>

static JsonNode *parse_value(Parser *parser);

static Token next_token(Parser *parser) {
  parser->token = parser->lookahead;
  parser->lookahead = lexer_next_token(&parser->lexer);
  return parser->token;
}

static JsonNode *make_node(JsonType type) {
  JsonNode *node = malloc(sizeof(JsonNode));
  if (node == NULL)
    return NULL;
  node->type = type;
  node->value = (JsonValue){0};
  return node;
}

static JsonNode *parse_string(Parser *parser) {
  JsonNode *node = make_node(JSON_STRING);
  if (node == NULL)
    return NULL;
  node->value.string = strdup(parser->token.value.string);
  free(parser->token.value.string);
  parser->token.value.string = NULL;
  next_token(parser);
  return node;
}

static JsonNode *parse_number(Parser *parser) {
  JsonNode *node = make_node(JSON_NUMBER);
  if (node == NULL)
    return NULL;
  node->value.number = parser->token.value.number;
  next_token(parser);
  return node;
}

static JsonNode *parse_bool(Parser *parser) {
  JsonNode *node = make_node(JSON_BOOL);
  if (node == NULL)
    return NULL;
  node->value.boolean = parser->token.type == JSON_TRUE;
  next_token(parser);
  return node;
}

static JsonNode *parse_null(Parser *parser) {
  JsonNode *node = make_node(JSON_NULL);
  if (node == NULL)
    return NULL;
  next_token(parser);
  return node;
}

static JsonNode *parse_array(Parser *parser) {
  JsonNode *node = make_node(JSON_ARRAY);
  if (node == NULL)
    return NULL;
  next_token(parser);

  size_t count = 0;
  size_t capacity = 4;
  JsonNode **items = malloc(capacity * sizeof(JsonNode *));

  while (parser->token.type != RSQUAREBRACKET &&
         parser->token.type != TOKEN_EOF && parser->token.type != TOKEN_ERROR) {
    if (count == capacity) {
      capacity <<= 1;
      JsonNode **tmp = realloc(items, capacity * sizeof(JsonNode *));
      if (tmp == NULL) {
        free(items);
        return NULL;
      }
      items = tmp;
    }
    JsonNode *item = parse_value(parser);
    if (item == NULL) {
      free(items);
      return NULL;
    }
    items[count++] = item;
    if (parser->token.type == COMMA)
      next_token(parser);
  }

  if (parser->token.type != RSQUAREBRACKET) {
    for (size_t i = 0; i < count; i++)
      json_free(items[i]);
    free(items);
    json_free(node);
    return NULL;
  }
  next_token(parser);

  if (count == 0) {
    free(items);
    node->value.array.items = NULL;
  } else {
    JsonNode **tmp = realloc(items, count * sizeof(JsonNode *));
    if (tmp != NULL)
      items = tmp;
    node->value.array.items = items;
  }
  node->value.array.count = count;
  return node;
}

static JsonPair *make_pair(Parser *parser) {
  if (parser->token.type != STRING)
    return NULL;
  JsonPair *pair = malloc(sizeof(JsonPair));
  if (pair == NULL)
    return NULL;

  pair->key = strdup(parser->token.value.string);
  free(parser->token.value.string);
  parser->token.value.string = NULL;
  next_token(parser);

  if (parser->token.type != COLON || parser->lookahead.type == TOKEN_ERROR) {
    free(pair->key);
    free(pair);
    return NULL;
  }
  next_token(parser);

  pair->value = parse_value(parser);
  if (pair->value == NULL) {
    free(pair->key);
    free(pair);
    return NULL;
  }
  return pair;
}

static JsonNode *parse_object(Parser *parser) {
  JsonNode *node = make_node(JSON_OBJECT);
  if (node == NULL)
    return NULL;
  next_token(parser);

  size_t count = 0;
  size_t capacity = 4;
  JsonPair *pairs = malloc(capacity * sizeof(JsonPair));
  if (pairs == NULL) {
    free(node);
    return NULL;
  }

  while (parser->token.type == STRING) {
    if (count == capacity) {
      capacity <<= 1;
      JsonPair *tmp = realloc(pairs, capacity * sizeof(JsonPair));
      if (tmp == NULL) {
        free(pairs);
        return NULL;
      }
      pairs = tmp;
    }
    JsonPair *pair = make_pair(parser);
    if (pair == NULL) {
      for (size_t j = 0; j < count; j++) {
        free(pairs[j].key);
        json_free(pairs[j].value);
      }
      free(pairs);
      json_free(node);
      return NULL;
    }
    pairs[count++] = *pair;
    free(pair);

    if (parser->token.type == COMMA)
      next_token(parser);
  }

  if (parser->token.type != RBRACE) {
    for (size_t i = 0; i < count; i++)
      free(pairs[i].key);
    free(pairs);
    json_free(node);
    return NULL;
  }
  next_token(parser);

  if (count == 0) {
    free(pairs);
    node->value.object.pairs = NULL;
  } else {
    JsonPair *tmp = realloc(pairs, count * sizeof(JsonPair));
    if (tmp != NULL)
      pairs = tmp;
    node->value.object.pairs = pairs;
  }
  node->value.object.count = count;
  return node;
}

static JsonNode *parse_value(Parser *parser) {
  switch (parser->token.type) {
  case STRING:
    return parse_string(parser);
  case NUMBER:
    return parse_number(parser);
  case JSON_TRUE:
  case JSON_FALSE:
    return parse_bool(parser);
  case TOKEN_NULL:
    return parse_null(parser);
  case LSQUAREBRACKET:
    return parse_array(parser);
  case LBRACE:
    return parse_object(parser);
  default:
    return NULL;
  }
}

JsonNode *parser_parse(Parser *parser) { return parse_value(parser); }

void parser_init(Parser *parser, const char *src) {
  lexer_init(&parser->lexer, src);
  parser->token = lexer_next_token(&parser->lexer);
  parser->lookahead = lexer_next_token(&parser->lexer);
}