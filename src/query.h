#ifndef QUERY_H
#define QUERY_H

#include "json.h"
#include <stdbool.h>

JsonNode *json_get_key(JsonNode *node, const char *key);
JsonNode *json_get_index(JsonNode *node, size_t i);
JsonNode *json_get_path(JsonNode *node, const char *path);
bool json_has_key(JsonNode *node, const char *key);

#endif