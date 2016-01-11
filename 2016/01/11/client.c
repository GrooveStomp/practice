#include "gs_socket.c"
#include <string.h> // strlen

int
main(int argc, char **argv) {
  bool result;

  int socket_handle;
  result = gs_socket_init(30000, &socket_handle);
  if (gs_socket_error(result)) {
    fprintf(stderr, "%s\n", gs_socket_message(result));
    return 1;
  }

  const char* packet_data = "Hello there!";
  int packet_size = strlen(packet_data);

  struct sockaddr_in address = gs_socket_address_init(30000, 127, 0, 0, 1);
  result = gs_socket_send(socket_handle, packet_data, packet_size, address);
  if (gs_socket_error(result)) {
    fprintf(stderr, "%s\n", gs_socket_message(result));
    return 1;
  }

  return 0;
}
