#ifndef QUERY_H
#define QUERY_H

#include "json.h"
#include <stdbool.h>

JsonNode *json_get_key(JsonNode *node, const char *key);
JsonNode *json_get_index(JsonNode *node, size_t i);
JsonNode *json_get_path(JsonNode *node, const char *path);
bool json_has_key(JsonNode *node, const char *key);

void json_append(JsonNode *array, JsonNode *item);
void json_set_index(JsonNode *array, size_t i, JsonNode *item);
void json_insert_index(JsonNode *array, size_t i, JsonNode *item);
void json_set_key(JsonNode *object, const char *key, JsonNode *value);
JsonNode *json_remove_index(JsonNode *array, size_t i);
JsonNode *json_remove_key(JsonNode *object, const char *key);
JsonNode *json_pop(JsonNode *array);
JsonNode *json_popleft(JsonNode *array);

#endif