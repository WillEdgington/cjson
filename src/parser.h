#ifndef PARSER_H
#define PARSER_H

#include "json.h"
#include "lexer.h"

typedef struct {
  Lexer lexer;
  Token token;
  Token lookahead;
} Parser;

void parser_init(Parser *parser, const char *src);
JsonNode *parser_parse(Parser *parser);

#endif