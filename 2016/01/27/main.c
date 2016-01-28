#include <stdio.h>
char foo = '"';
/* Whoa! */
#include <stdbool.h>

#include "file_buffer.c"

#define BUFFER_SIZE kilobytes(30)
#define ARRAY_SIZE(Array) (sizeof((Array)) / sizeof((Array)[0]))

void aro_abort(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

void usage() {
  printf("Usage: program file\n");
  printf("  file: must be a file in this directory\n");
  printf("  Specify '-h' or '--help' for this help text\n");
  exit(EXIT_SUCCESS);
}

bool get_first_character_literal(const buffer_t *buff, buffer_t *character) {
  char *string_open_match;
  char *string_close_match;

  /* TODO(AARON): Assuming entirety of string fits within both buffer_t and tmp_buffer. */
  char tmp_buffer[5] = {0};
  strncpy(tmp_buffer, buff->data, min(buff->size, 4));

  char *match;
  if ((match = strstr(tmp_buffer, "'")) != NULL) {
    char *close = match + 2;
    if (*close != '\'') close += 1;
    if (*close != '\'') return false;
    if (*close == '\'' && *(close-1) == '\\') close += 1;
    if (*close != '\'') return false;

    character->data = buff->data + (match - tmp_buffer);
    character->size = close - match + 1;
    character->max_size = character->size;
    return true;
  }

  return false;
}

bool get_first_string_literal(const buffer_t *buff, buffer_t *string) {
  char *string_open_match;
  char *string_close_match;

  /* TODO(AARON): Assuming entirety of string fits within both buffer_t and tmp_buffer. */
  char tmp_buffer[kilobytes(1)] = {0};
  strncpy(tmp_buffer, buff->data, min(buff->size, kilobytes(1)-1));

  if ((string_open_match = strstr(tmp_buffer, "\"")) != NULL) {
    /* TODO(AARON): Handle nested strings. */
    string_close_match = strstr(tmp_buffer + 1, "\"");
    char *close_tmp_buffer;
    while (*(string_close_match - 1) == '\\' || strncmp(string_close_match - 1, "'\"'", 4) == 0) {
      close_tmp_buffer = string_close_match + 1;

      if (close_tmp_buffer >= tmp_buffer + min(buff->size, kilobytes(1)-1))
        aro_abort("Couldn't terminate string properly");

      string_close_match = strstr(close_tmp_buffer, "\"");
    }
    /* TODO(AARON): Raise a big fuss if string_close_match is NULL. */

    string->data = buff->data + (string_open_match - tmp_buffer);
    string->size = string_close_match - string_open_match + 1;
    string->max_size = string->size;

    return true;
  }

  return false;
}

bool get_first_comment(const buffer_t *buff, buffer_t *comment) {
  char *comment_open_match;
  char *comment_close_match;

  /* TODO(AARON): Assuming entirety of comment fits within both buffer_t and tmp_buffer. */
  char tmp_buffer[kilobytes(1)] = {0};
  strncpy(tmp_buffer, buff->data, min(buff->size, kilobytes(1)-1));

  if ((comment_open_match = strstr(tmp_buffer, "/*")) != NULL) {
    /* TODO(AARON): Handle nested comments. */
    comment_close_match = strstr(tmp_buffer, "*/");
    /* TODO(AARON): Raise a big fuss if comment_close_match is NULL. */

    comment->data = buff->data + (comment_open_match - tmp_buffer);
    comment->size = comment_close_match - comment_open_match + 2;
    comment->max_size = comment->size;

    return true;
  }

  return false;
}

bool get_first_keyword(const buffer_t *buff, buffer_t *keyword) {
  static char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
  };

  char tmp_buffer[20] = {0}; /* NOTE(AARON): No keyword is even close to 20 characters long. */
  strncpy(tmp_buffer, buff->data, min(buff->size, 19));

  char *match;
  for (int i=0; i < ARRAY_SIZE(keywords); ++i) {
    if ((match = strstr(tmp_buffer, keywords[i])) != NULL) {
      keyword->data = buff->data + (match - tmp_buffer);
      keyword->size = strlen(keywords[i]);
      keyword->max_size = keyword->size;
      return true;
    }
  }

  return false;
}

bool is_symbol(const char character) {
  static char symbols[] = {
    '#', '{', '}', '[', ']', ',', ';', '-', '+', '=', '*', '^', '&', '%', '$', '?', '<', '>', '(', ')', '!',
    '/', '|', '~', '.', '\'', '"'
  };

  for (int i=0; i < ARRAY_SIZE(symbols); ++i) {
    if (character == symbols[i]) return true;
  }
}

void inc_buffer(buffer_t *buff, unsigned int count) {
  buff->data += count;
  buff->size -= count;
  buff->max_size -= count;
}

int main(int argc, char *argv[]) {
  if (argc != 2 || argv[1] == "-h" || argv[1] == "--help") usage();

  char memory[BUFFER_SIZE] = {0};
  buffer_t buffer;
  buffer_t_set(&buffer, memory, BUFFER_SIZE, 0);

  FILE *to_lex = fopen(argv[1], "r"); /* argv[1] is the input file name. */

  if (copy_file_to_buffer(to_lex, &buffer) != COPY_FILE_OK)
    aro_abort("Couldn't copy entire file to buffer");

  char print_buffer[kilobytes(1)] = { 0 };
  int pos = 0;

  while(buffer.size > 0) {
    buffer_t match;
    if (get_first_keyword(&buffer, &match) && match.data == buffer.data) {
      snprintf(print_buffer, match.size + strlen("<keyword>") + 1, "<keyword>%s", match.data);
      puts(print_buffer);
      inc_buffer(&buffer, match.size);
      pos += match.size;
    }
    else if (get_first_character_literal(&buffer, &match) && match.data == buffer.data) {
      snprintf(print_buffer, match.size + strlen("<char>") + 1, "<char>%s", match.data);
      puts(print_buffer);
      inc_buffer(&buffer, match.size);
      pos += match.size;
    }
    else if (get_first_comment(&buffer, &match) && match.data == buffer.data) {
      snprintf(print_buffer, match.size + strlen("<comment>") + 1, "<comment>%s", match.data);
      puts(print_buffer);
      inc_buffer(&buffer, match.size);
      pos += match.size;
    }
    else if (get_first_string_literal(&buffer, &match) && match.data == buffer.data) {
      snprintf(print_buffer, match.size + strlen("<string>") + 1, "<string>%s", match.data);
      puts(print_buffer);
      inc_buffer(&buffer, match.size);
      pos += match.size;
    }
    else {
      /* printf("No token found at character [%u]\n", pos); */
      inc_buffer(&buffer, 1);
      pos += 1;
    }
  }

  return 0;
}
