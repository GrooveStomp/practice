#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define bytes(n) (n)
#define kilobytes(n) (bytes(n) * 1000)

typedef struct {
  char *data;
  size_t max_size;
  size_t size;
} buffer_t;

buffer_t *buffer_t_set(buffer_t *buff, char *buff_data, size_t buff_max_size, size_t buff_size) {
  buff->data = buff_data;
  buff->max_size = buff_max_size;
  buff->size = buff_size;
  return buff;
}

void usage() {
  printf("Usage: program buffer_size file\n");
  printf("  buffer_size: size of file buffer in bytes to load file data into\n");
  printf("  file: must be a file in this directory\n");
  printf("  Specify '-h' or '--help' for this help text\n");
  exit(EXIT_SUCCESS);
}

typedef enum {
  COPY_FILE_OK,
  COPY_FILE_DATA_REMAINING,
  COPY_FILE_ERROR
} copy_file_result_t;

copy_file_result_t copy_file_to_buffer(FILE *source, buffer_t *dest) {
  size_t tmp_buffer_size = kilobytes(1);
  char tmp_buffer[tmp_buffer_size+1];
  char *fgets_result;

  while(1) {
    memset(tmp_buffer, 0, tmp_buffer_size + 1);
    if (dest->max_size == dest->size && fgets_result != NULL) {
      return COPY_FILE_DATA_REMAINING;
    }

    /* dest->max_size - dest->size + 1 to account for trailing null in source. */
    size_t size_to_copy = min(tmp_buffer_size, dest->max_size - dest->size + 1);
    fgets_result = fgets(tmp_buffer, size_to_copy, source);
    if (fgets_result == NULL && ferror(source)) {
      return COPY_FILE_ERROR;
    }

    size_to_copy = min(dest->max_size - dest->size, strlen(tmp_buffer));
    strncpy(dest->data + dest->size, tmp_buffer, size_to_copy);
    dest->size += size_to_copy;
    if (size_to_copy < strlen(tmp_buffer)) {
      /*
        TODO(AARON, 2016-01-24): Reset file pointer if all data isn't copied.
        Make sure that after we've filled dest, that we reset the cursor in source appropriately.
      */
      return COPY_FILE_DATA_REMAINING;
    }
    else if (fgets_result == NULL) {
      return COPY_FILE_OK;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3 || argv[1] == "-h" || argv[1] == "--help") { usage(); }

  unsigned int memory_size = bytes(atoi(argv[1]));
  char memory[memory_size];
  memset(memory, 0, memory_size);

  buffer_t buffer;
  buffer_t_set(&buffer, memory, memory_size, 0);

  FILE *to_lex = fopen(argv[2], "r"); /* argv[2] is the input file name. */

  /*
    NOTE(AARON, 2016-01-24): Just copy memory_size bytes from file.
    There's no reason to do this other than it's easier and lets me carry on.
    At some point I definitely need to loop and buffer appropriately, taking
    into consideration start/end of token and shifting the buffer appropriately.
  */
  copy_file_result_t copy_result = copy_file_to_buffer(to_lex, &buffer);

  switch(copy_result) {
    case(COPY_FILE_OK): {
      printf("Copied file successfully to buffer.\n");
      break;
    }
    case(COPY_FILE_DATA_REMAINING): {
      printf("Copied portion of file successfully. Data remaining.\n");
      break;
    }
    case(COPY_FILE_ERROR): {
      fprintf(stderr, "Error copying file to buffer.\n");
      exit(EXIT_FAILURE);
      break;
    }
  }

  return 0;
}
