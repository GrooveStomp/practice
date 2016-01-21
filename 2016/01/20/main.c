#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))

#define SIGIL_START "{{"
#define SIGIL_END   "}}"

struct buffer {
  char *data;
  unsigned int size;
};

struct buffer lookup(struct buffer stream) {
  struct buffer result;

  static char *url = "https://www.groovestomp.com";
  static char *label = "My Website";

  if (strncmp(stream.data, "url", min(stream.size, strlen("url"))) == 0) {
    result.data = url;
    result.size = strlen(url);
  }
  else if (strncmp(stream.data, "label", min(stream.size, strlen("label"))) == 0) {
    result.data = label;
    result.size = strlen(label);
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
  for(; *tail == ' '; --tail);
  struct buffer result;
  result.data = head;
  result.size = tail - head;
  return result;
}

uint8_t memory[10000] = { 0 };
char *front_buffer = (char *)memory;
char *back_buffer = (char *)(memory + 5000);
char *swap;

int main(int argc, char *argv[])
{
  /* To Do: Output usage information. */
  if (argc != 2) { return 1; }

  char *replace_start, *replace_end, *stream;
  stream = argv[1];

  while (1) {
    struct buffer to_replace, trimmed, replaced;
    replace_start = strstr(stream, SIGIL_START);
    replace_end = strstr(stream, SIGIL_END);

    if (replace_start == NULL || replace_end == NULL) break;
    replace_end += 1;

    to_replace.data = replace_start;
    to_replace.size = replace_end - replace_start;
    trimmed = trim(to_replace);
    replaced = lookup(trimmed);

    strncpy(front_buffer, back_buffer, strlen(back_buffer));
    strncpy(front_buffer + strlen(front_buffer), stream, replace_start - stream);
    strncpy(front_buffer + strlen(front_buffer), replaced.data, replaced.size);

    swap = back_buffer;
    back_buffer = front_buffer;
    front_buffer = swap;

    stream = replace_end + 1;
  }

  strncpy(front_buffer, back_buffer, strlen(back_buffer));
  strncpy(front_buffer + strlen(front_buffer), stream, strlen(stream));

  printf("%s\n", front_buffer);

  return(0);
}
