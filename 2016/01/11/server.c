#include "gs_socket.c"
#include <string.h> // memset

int
main(int argc, char **argv) {
  bool result;
  uint8_t packet_data[256];
  uint32_t max_packet_size = sizeof(packet_data);
  struct sockaddr_in from;
  socklen_t from_length = sizeof(from);

  int socket_handle;
  result = gs_socket_init(30000, &socket_handle);
  if (gs_socket_error(result)) {
    fprintf(stderr, "%s\n", gs_socket_message(result));
    return 1;
  }

  while(true) {
    while(true) {
      memset((void*)packet_data, 0, max_packet_size);
      int bytes = recvfrom(socket_handle, (char*)packet_data, max_packet_size, 0, (struct sockaddr*)&from, &from_length);
      if (bytes <= 0) break;

      uint32_t from_address = ntohl(from.sin_addr.s_addr);
      uint32_t from_port = ntohs(from.sin_port);

      fprintf(stdout, "Recieved data: port:%d, address: %d, data: %s\n", from_port, from_address, packet_data);
    }
  }

  return 0;
}
