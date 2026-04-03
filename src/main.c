#include "json.h"
#include "parser.h"
#include "query.h"
#include "read.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: cjson <file> [path]\n");
    return 1;
  }

  char *src = read_file(argv[1]);
  if (src == NULL) {
    printf("error: could not read file %s\n", argv[1]);
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

  if (argc >= 3) {
    JsonNode *result = json_get_path(root, argv[2]);
    if (result) {
      json_print(result);
      printf("\n");
    } else {
      printf("Path not found or invalid.\n");
    }
  } else {
    json_print(root);
    printf("\n");
  }

  json_free(root);
  return 0;
}