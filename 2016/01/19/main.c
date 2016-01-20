#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

uint32_t footnote(const char *id_prefix, const char *in_text, char **footnote_out, char **link_out) {
  static int footnote_index = 1;
  char footnote_index_text[255] = { 0 };
  sprintf(footnote_index_text, "%u", footnote_index);

  /*
    <sub>
      <sup id="id_prefix_ref1">1</sup>
      in_text
    </sub>

    <sup><a href="#id_prefix_ref1">1</a></sup>

    printf("length: %u\n", strlen("<sub><sup id=\"_ref\"></sup></sub>"));
    > 32
    printf("length: %u\n", strlen("<sup><a href=\"#_ref\"></a></sup>"));
    > 31
  */

  size_t malloc_size;

  malloc_size = strlen(id_prefix) + strlen(in_text) + strlen(footnote_index_text) + 33 + 1;
  *footnote_out = malloc(malloc_size);
  memset(*footnote_out, 0, malloc_size);
  snprintf(*footnote_out, malloc_size, "<sub><sup id=\"%s_ref%u\">%u</sup>%s</sub>", id_prefix, footnote_index, footnote_index, in_text);

  malloc_size = strlen(id_prefix) + strlen(footnote_index_text) + 32 + 1;
  *link_out = malloc(malloc_size);
  memset(*link_out, 0, malloc_size);
  snprintf(*link_out, malloc_size, "<sup><a href=\"#%s_ref%u\">%u</a></sup>", id_prefix, footnote_index, footnote_index);

  return footnote_index++;
}

int main(int argc, char *argv[]) {
  char *footnote_text;
  char *link_text;

  footnote("2016-01-19", "Mike Acton's interview at HandmadeCon", &footnote_text, &link_text);

  puts(footnote_text);
  puts("");
  puts(link_text);
  puts("");

  footnote("2016-01-19", "My own, personal website", &footnote_text, &link_text);

  puts(footnote_text);
  puts("");
  puts(link_text);
  puts("");

  return 0;
}
