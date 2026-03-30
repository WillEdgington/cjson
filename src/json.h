#ifndef JSON_H
#define JSON_H

#include <stdbool.h>
#include <stddef.h>

typedef struct JsonNode JsonNode;

typedef struct {
  char *key;
  JsonNode *value;
} JsonPair;

typedef union {
  bool boolean;
  double number;
  char *string;
  struct {
    JsonNode **items;
    size_t count;
  } array;
  struct {
    JsonPair *pairs;
    size_t count;
  } object;
} JsonValue;

typedef enum {
  JSON_BOOL,
  JSON_NUMBER,
  JSON_STRING,
  JSON_NULL,
  JSON_ARRAY,
  JSON_OBJECT,
} JsonType;

struct JsonNode {
  JsonType type;
  JsonValue value;
};

#define INDENT 2

void json_free(JsonNode *jsonnode);
void json_print(JsonNode *jsonnode);

#endif