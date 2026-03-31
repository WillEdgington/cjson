#include "read.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return NULL;

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return NULL;
  }

  long filesize = ftell(file);
  if (filesize < 0) {
    fclose(file);
    return NULL;
  }

  rewind(file);

  char *content = malloc(filesize + 1);
  if (content == NULL) {
    fclose(file);
    return NULL;
  }

  size_t readsize = fread(content, 1, filesize, file);
  if (readsize != (size_t)filesize) {
    free(content);
    fclose(file);
    return NULL;
  }
  content[filesize] = '\0';
  fclose(file);

  return content;
}