#ifndef _GC_COMMON_C
#define _GS_COMMON_C

/* Stolen from: http://stackoverflow.com/a/3437484/321897 */
#define min(a,b) ((a) < (b) ? (a) : (b))

#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t GSNET_PROTOCOL_ID = 2700;
#define GSNET_PACKET_MAX_SIZE 256
#define GSNET_PACKET_DATA_SIZE (GSNET_PACKET_MAX_SIZE - sizeof(GSNET_PROTOCOL_ID))

typedef enum {
  GSNET_ERR_NONE,
  GSNET_ERR_BUFFER_OVERFLOW,
} gsnet_error_t;

struct gsnet_connection {
  int address;
  int port;
  int elapsed_time_seconds;
};

struct gsnet_connection gsnet_null_connection = { 0 };

char *
gs_gets(char *s, int n) {
  char *res = fgets(s, n, stdin);
  if (NULL == res) return res;

  char *newline = memchr(s, '\n', n);
  if (NULL != newline) { *newline = '\0'; }

  return res;
}

void
gsnet_init_packet(void *packet) {
  *(char *)packet = GSNET_PROTOCOL_ID;
}

gsnet_error_t
gsnet_set_packet_data(void *packet, const void *data, size_t data_size_in_bytes) {
  if (data_size_in_bytes > GSNET_PACKET_DATA_SIZE) {
    return GSNET_ERR_BUFFER_OVERFLOW;
  }

  size_t size = min(data_size_in_bytes, GSNET_PACKET_DATA_SIZE);
  char *data_ptr = (char *)packet + sizeof(GSNET_PROTOCOL_ID);
  strncpy(data_ptr, data, size);
  return GSNET_ERR_NONE;
}

const char *
gsnet_get_packet_data(const void *packet) {
  return (char *)packet + sizeof(GSNET_PROTOCOL_ID);
}

void
gsnet_error(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}

#endif /* _GS_COMMON_C */
