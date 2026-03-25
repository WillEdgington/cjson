#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
  LBRACE,
  RBRACE,
  LSQUAREBRACKET,
  RSQUAREBRACKET,
  COMMA,
  COLON,
  STRING,
  NUMBER,
  JSON_TRUE,
  JSON_FALSE,
  JSON_NULL,
  TOKEN_EOF,
} TokenType;

typedef union {
  char *string;
  double number;
} TokenValue;

typedef struct {
  TokenType type;
  TokenValue value;
} Token;

typedef struct {
  const char *src;
  size_t pos;
} Lexer;

void lexer_init(Lexer *lexer, const char *src);
Token lexer_next_token(Lexer *lexer);

#endif
