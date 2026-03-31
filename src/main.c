#include "json.h"
#include "parser.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: cjson <file>\n");
    return 1;
  }

  char *src = read_file(argv[1]);
  if (src == NULL) {
    printf("error: could not read file\n");
    return 1;
  }

  Parser parser;
  parser_init(&parser, src);
  JsonNode *root = parser_parse(&parser);
  free(src);
  if (root == NULL) {
    printf("parse error\n");
    return 1;
  }

  json_print(root);
  printf("\n");
  json_free(root);
  return 0;
}