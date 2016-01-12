#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

const char *path = "/tmp/udp_sock_server";

void
rem_file(void) {
  if (remove(path) == -1 && errno != ENOENT) {
    fprintf(stderr, "Failed to remove file: %s\n", path);
  }
}

int
main(int argc, char **argv) {
  atexit(&rem_file);

  uint8_t packet_data[256];
  uint32_t max_packet_size = sizeof(packet_data);

  struct sockaddr_un address;
  memset(&address, 0, sizeof(struct sockaddr_un));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, path, strlen(path));

  int socket_handle;
  if ((socket_handle = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
  }

  if (bind(socket_handle, (struct sockaddr *)&address, sizeof(struct sockaddr_un)) == -1) {
    fprintf(stderr, "Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1) {
    fprintf(stderr, "Couldn't make socket non-blocking\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    while(1) {
      memset((void*)packet_data, 0, max_packet_size);

      if (recvfrom(socket_handle, (char*)packet_data, max_packet_size, 0, NULL, NULL) <= 0)
        break;

      fprintf(stdout, "Received data: %s\n", packet_data);
    }
  }

  exit(EXIT_SUCCESS);
}
