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

void json_append(JsonNode *array, JsonNode *item) {
  if (array->type != JSON_ARRAY)
    return;
  JsonNode **tmp =
      array->value.array.items != NULL
          ? realloc(array->value.array.items,
                    (array->value.array.count + 1) * sizeof(JsonNode *))
          : malloc(sizeof(JsonNode *));

  if (tmp == NULL)
    return;

  tmp[array->value.array.count++] = item;
  array->value.array.items = tmp;
}

void json_insert_index(JsonNode *array, size_t i, JsonNode *item) {
  if (array->type != JSON_ARRAY || i >= array->value.array.count)
    return;

  JsonNode **tmp = realloc(array->value.array.items,
                           (array->value.array.count + 1) * sizeof(JsonNode *));
  if (!tmp)
    return;
  array->value.array.items = tmp;

  for (size_t j = array->value.array.count; j > i; j--) {
    array->value.array.items[j] = array->value.array.items[j - 1];
  }

  array->value.array.items[i] = item;
  array->value.array.count++;
}

void json_set_index(JsonNode *array, size_t i, JsonNode *item) {
  if (array->type != JSON_ARRAY || i >= array->value.array.count)
    return;
  json_free(array->value.array.items[i]);
  array->value.array.items[i] = item;
}

JsonNode *json_remove_key(JsonNode *object, const char *key) {
  if (object->type != JSON_OBJECT || !json_has_key(object, key))
    return NULL;

  JsonPair *tmp = malloc((object->value.object.count - 1) * sizeof(JsonPair));
  if (tmp == NULL)
    return NULL;

  JsonNode *val = NULL;
  size_t ptr = 0;
  for (size_t i = 0; i < object->value.object.count; i++) {
    if (strcmp(object->value.object.pairs[i].key, key) == 0) {
      val = object->value.object.pairs[i].value;
      free(object->value.object.pairs[i].key);
      continue;
    }
    tmp[ptr++] = object->value.object.pairs[i];
  }
  free(object->value.object.pairs);
  object->value.object.pairs = tmp;
  object->value.object.count--;
  return val;
}

void json_set_key(JsonNode *object, const char *key, JsonNode *value) {
  if (object->type != JSON_OBJECT)
    return;

  JsonNode *val = json_remove_key(object, key);
  if (val != NULL)
    json_free(val);
  JsonPair *tmp =
      object->value.object.pairs != NULL
          ? realloc(object->value.object.pairs,
                    (object->value.object.count + 1) * sizeof(JsonPair))
          : malloc(sizeof(JsonPair));

  if (tmp == NULL)
    return;

  tmp[object->value.object.count].key = strdup(key);
  tmp[object->value.object.count++].value = value;
  object->value.object.pairs = tmp;
}

JsonNode *json_pop(JsonNode *array) {
  if (array->type != JSON_ARRAY || array->value.array.count == 0)
    return NULL;

  array->value.array.count--;
  JsonNode *item = array->value.array.items[array->value.array.count];
  JsonNode **tmp = realloc(array->value.array.items,
                           array->value.array.count * sizeof(JsonNode *));
  if (tmp == NULL) {
    array->value.array.count++;
    return NULL;
  }

  array->value.array.items = tmp;
  return item;
}

JsonNode *json_remove_index(JsonNode *array, size_t i) {
  if (array->type != JSON_ARRAY || array->value.array.count <= i)
    return NULL;

  if (array->value.array.count == i + 1) {
    return json_pop(array);
  }

  JsonNode **tmp = malloc((array->value.array.count - 1) * sizeof(JsonNode *));
  if (tmp == NULL)
    return NULL;

  for (size_t ptr = 0; ptr < array->value.array.count; ptr++) {
    if (ptr < i) {
      tmp[ptr] = array->value.array.items[ptr];
    } else if (ptr > i) {
      tmp[ptr - 1] = array->value.array.items[ptr];
    }
  }
  JsonNode *item = array->value.array.items[i];
  free(array->value.array.items);
  array->value.array.items = tmp;
  array->value.array.count--;
  return item;
}

JsonNode *json_popleft(JsonNode *array) { return json_remove_index(array, 0); }
