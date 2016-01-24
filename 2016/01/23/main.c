#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "hash_map.c"

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SIGIL_START "{{"
#define SIGIL_END   "}}"
#define EMPTY_STRING ""

struct buffer {
  char *data;
  unsigned int size;
};

struct buffer lookup(struct hash_map *env, struct buffer stream) {
  char tmp_str[255] = { 0 };
  void *hash_result;
  struct buffer result;

  strncpy(tmp_str, stream.data, stream.size);
  if (hash_map_get(env, tmp_str, &hash_result)) {
    result.data = (char *)hash_result;
    result.size = strlen((char *)hash_result);
  }
  else {
    result.data = NULL;
    result.size = 0;
  }

  return result;
}

struct buffer trim(struct buffer stream) {
  char *head = stream.data + 2;
  char *tail = stream.data + stream.size - 2;
  for(; *head == ' '; ++head);
  for(; *tail == ' '; --tail); /* Tail now points to last character. */
  struct buffer result;
  result.data = head;
  result.size = (tail+1) - head;
  return result;
}

void usage() {
  puts("Usage: program [env pairs] text");
}

uint8_t memory[10000] = { 0 };
char *front_buffer = (char *)memory;
char *back_buffer = (char *)(memory + 5000);
char *swap;

int main(int argc, char *argv[])
{
  if ((argc - 2) % 2 != 0) {
    usage();
    exit(EXIT_FAILURE);
  }

  void *result;

  struct hash_map *env = hash_map_create();
  for (int i=1; i < argc-1; i+=2) {
    hash_map_add(env, argv[i], (void *)argv[i+1]);
  }

  for (int i=1; i < argc-1; i+=2) {
    hash_map_get(env, argv[i], &result);
    printf("hash_map[%s]: %s\n", argv[i], (char *)result);
  }

  char *replace_start, *replace_end, *stream;
  stream = argv[argc-1];

  while (1) {
    struct buffer to_replace, trimmed, replaced;
    replace_start = strstr(stream, SIGIL_START);
    replace_end = strstr(stream, SIGIL_END);

    if (replace_start == NULL || replace_end == NULL) break;
    replace_end += 2; /* Account for SIGIL_END: "}}" */

    to_replace.data = replace_start;
    to_replace.size = replace_end - replace_start - 1;
    trimmed = trim(to_replace);
    replaced = lookup(env, trimmed);

    strncpy(front_buffer, back_buffer, strlen(back_buffer));
    strncpy(front_buffer + strlen(front_buffer), stream, replace_start - stream);
    strncpy(front_buffer + strlen(front_buffer), replaced.data, replaced.size);

    swap = back_buffer;
    back_buffer = front_buffer;
    front_buffer = swap;

    stream = replace_end;
  }

  strncpy(front_buffer, back_buffer, strlen(back_buffer));
  strncpy(front_buffer + strlen(front_buffer), stream, strlen(stream));

  printf("%s\n", front_buffer);

  return(0);
}
