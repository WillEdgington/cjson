#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
  Lexer lexer;
  Token token;
  Token lookahead;
} Parser;

#endif