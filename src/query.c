#include "query.h"
#include <stdlib.h>
#include <string.h>

JsonNode *json_get_key(JsonNode *node, const char *key) {
  if (node->type != JSON_OBJECT) {
    return NULL;
  }

  for (size_t i = 0; i < node->value.object.count; i++) {
    if (strcmp(node->value.object.pairs[i].key, key) == 0) {
      return node->value.object.pairs[i].value;
    }
  }

  return NULL;
}

JsonNode *json_get_index(JsonNode *node, size_t i) {
  if (node->type != JSON_ARRAY || node->value.array.count <= i) {
    return NULL;
  }

  return node->value.array.items[i];
}

bool json_has_key(JsonNode *node, const char *key) {
  if (node->type != JSON_OBJECT) {
    return false;
  }

  for (size_t i = 0; i < node->value.object.count; i++) {
    if (strcmp(node->value.object.pairs[i].key, key) == 0) {
      return true;
    }
  }

  return false;
}

JsonNode *json_get_path(JsonNode *node, const char *path) {
  if (node->type != JSON_ARRAY && node->type != JSON_OBJECT) {
    return NULL;
  }

  JsonNode *cur = node;
  const char *segment = path;

  while (segment != NULL) {
    const char *dot = strchr(segment, '.');
    size_t length = dot ? (size_t)(dot - segment) : strlen(segment);

    char *key = malloc(length + 1);
    if (key == NULL)
      return NULL;
    memcpy(key, segment, length);
    key[length] = '\0';

    switch (cur->type) {
    case JSON_ARRAY: {
      char *endptr;
      size_t idx = strtoul(key, &endptr, 10);
      if (endptr == key) {
        free(key);
        return NULL;
      }
      cur = json_get_index(cur, idx);
      break;
    }
    case JSON_OBJECT:
      cur = json_get_key(cur, key);
      break;
    default:
      free(key);
      return NULL;
    }
    free(key);

    if (cur == NULL)
      return NULL;

    segment = dot ? dot + 1 : NULL;
  }

  return cur;
}