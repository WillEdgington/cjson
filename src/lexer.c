#include "lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static void skip_whitespace(Lexer *lexer) {
  while (isspace(lexer->src[lexer->pos]))
    lexer->pos++;
}

static Token make_token(TokenType type) {
  Token token = {0};
  token.type = type;
  return token;
}

static Token scan_string(Lexer *lexer) {
  lexer->pos++;
  size_t start = lexer->pos;

  while (lexer->src[lexer->pos] != '"' && lexer->src[lexer->pos] != '\0')
    lexer->pos++;

  if (lexer->src[lexer->pos] == '\0')
    return make_token(TOKEN_ERROR);

  size_t end = lexer->pos;
  size_t length = end - start;

  char *str = malloc(length + 1);
  if (str == NULL)
    return make_token(TOKEN_ERROR);

  memcpy(str, lexer->src + start, length);
  str[length] = '\0';
  lexer->pos++;

  Token token = make_token(STRING);
  token.value.string = str;
  return token;
}

static Token scan_number(Lexer *lexer) {
  const char *start = lexer->src + lexer->pos;
  char *end;
  double number = strtod(start, (char **)&end);

  if (start == end)
    return make_token(TOKEN_ERROR);
  lexer->pos += (size_t)(end - start);

  Token token = make_token(NUMBER);
  token.value.number = number;
  return token;
}

static Token scan_keyword(Lexer *lexer) {
  if (strncmp(lexer->src + lexer->pos, "true", 4) == 0) {
    lexer->pos += 4;
    return make_token(JSON_TRUE);
  } else if (strncmp(lexer->src + lexer->pos, "false", 5) == 0) {
    lexer->pos += 5;
    return make_token(JSON_FALSE);
  } else if (strncmp(lexer->src + lexer->pos, "null", 4) == 0) {
    lexer->pos += 4;
    return make_token(JSON_NULL);
  }
  return make_token(TOKEN_ERROR);
}

static Token scan_special(Lexer *lexer) {
  TokenType tokentype;
  switch (lexer->src[lexer->pos]) {
  case '{':
    tokentype = LBRACE;
    break;
  case '}':
    tokentype = RBRACE;
    break;
  case '[':
    tokentype = LSQUAREBRACKET;
    break;
  case ']':
    tokentype = RSQUAREBRACKET;
    break;
  case ',':
    tokentype = COMMA;
    break;
  case ':':
    tokentype = COLON;
    break;
  case '\0':
    tokentype = TOKEN_EOF;
    break;
  default:
    tokentype = TOKEN_ERROR;
    break;
  }
  if (tokentype != TOKEN_ERROR && tokentype != TOKEN_EOF)
    lexer->pos++;
  return make_token(tokentype);
}

Token lexer_next_token(Lexer *lexer) {
  skip_whitespace(lexer);
  switch (lexer->src[lexer->pos]) {
  case '"':
    return scan_string(lexer);
  case 't':
  case 'f':
  case 'n':
    return scan_keyword(lexer);
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '-':
    return scan_number(lexer);
  case '{':
  case '}':
  case '[':
  case ']':
  case ',':
  case ':':
  case '\0':
    return scan_special(lexer);
  default:
    lexer->pos++;
    return make_token(TOKEN_ERROR);
  }
}

void lexer_init(Lexer *lexer, const char *src) {
  assert(lexer != NULL);
  assert(src != NULL);
  lexer->pos = 0;
  lexer->src = src;
}