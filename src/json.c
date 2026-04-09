#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_value(JsonNode *node, int depth);

static void print_indent(int depth) {
  for (int i = 0; i < INDENT * depth; i++)
    printf(" ");
}

static void print_string(JsonNode *node) {
  printf("\"%s\"", node->value.string);
}

static void print_number(JsonNode *node) { printf("%g", node->value.number); }

static void print_bool(JsonNode *node) {
  printf("%s", node->value.boolean ? "true" : "false");
}

static void print_null(void) { printf("null"); }

static void print_array(JsonNode *node, int depth) {
  JsonNode **items = node->value.array.items;
  printf("[\n");

  for (size_t i = 0; i < node->value.array.count; i++) {
    print_indent(depth + 1);
    print_value(items[i], depth + 1);
    if (i < node->value.array.count - 1)
      printf(",");
    printf("\n");
  }

  print_indent(depth);
  printf("]");
}

static void print_object(JsonNode *node, int depth) {
  JsonPair *pairs = node->value.object.pairs;
  printf("{\n");

  for (size_t i = 0; i < node->value.object.count; i++) {
    print_indent(depth + 1);
    printf("\"%s\": ", pairs[i].key);
    print_value(pairs[i].value, depth + 1);
    if (i < node->value.object.count - 1)
      printf(",");
    printf("\n");
  }

  print_indent(depth);
  printf("}");
}

static void print_value(JsonNode *node, int depth) {
  switch (node->type) {
  case JSON_BOOL:
    print_bool(node);
    break;
  case JSON_NUMBER:
    print_number(node);
    break;
  case JSON_STRING:
    print_string(node);
    break;
  case JSON_NULL:
    print_null();
    break;
  case JSON_ARRAY:
    print_array(node, depth);
    break;
  case JSON_OBJECT:
    print_object(node, depth);
    break;
  }
}

static void free_array(JsonNode *jsonnode) {
  for (size_t i = 0; i < jsonnode->value.array.count; i++)
    json_free(jsonnode->value.array.items[i]);
  free(jsonnode->value.array.items);
  free(jsonnode);
}

static void free_object(JsonNode *jsonnode) {
  for (size_t i = 0; i < jsonnode->value.object.count; i++) {
    free(jsonnode->value.object.pairs[i].key);
    json_free(jsonnode->value.object.pairs[i].value);
  }
  free(jsonnode->value.object.pairs);
  free(jsonnode);
}

void json_print(JsonNode *jsonnode) { print_value(jsonnode, 0); }

void json_free(JsonNode *jsonnode) {
  if (jsonnode == NULL)
    return;
  switch (jsonnode->type) {
  case JSON_ARRAY:
    free_array(jsonnode);
    break;
  case JSON_OBJECT:
    free_object(jsonnode);
    break;
  case JSON_STRING:
    free(jsonnode->value.string);
    free(jsonnode);
    break;
  default:
    free(jsonnode);
    break;
  }
}

JsonNode *json_make_node(JsonType type) {
  JsonNode *node = malloc(sizeof(JsonNode));
  if (node == NULL)
    return NULL;
  node->type = type;
  node->value = (JsonValue){0};
  return node;
}

JsonNode *json_make_string(const char *str) {
  JsonNode *node = json_make_node(JSON_STRING);
  if (node == NULL)
    return NULL;
  node->value.string = strdup(str);
  return node;
}
JsonNode *json_make_number(double num) {
  JsonNode *node = json_make_node(JSON_NUMBER);
  if (node == NULL)
    return NULL;
  node->value.number = num;
  return node;
}
JsonNode *json_make_bool(bool boolean) {
  JsonNode *node = json_make_node(JSON_BOOL);
  if (node == NULL)
    return NULL;
  node->value.boolean = boolean;
  return node;
}

JsonNode *json_make_null(void) {
  JsonNode *node = json_make_node(JSON_NULL);
  if (node == NULL)
    return NULL;
  return node;
}

JsonNode *json_make_array(void) {
  JsonNode *node = json_make_node(JSON_ARRAY);
  if (node == NULL)
    return NULL;
  node->value.array.count = 0;
  node->value.array.items = NULL;
  return node;
}

JsonNode *json_make_object(void) {
  JsonNode *node = json_make_node(JSON_OBJECT);
  if (node == NULL)
    return NULL;
  node->value.object.count = 0;
  node->value.object.pairs = NULL;
  return node;
}
