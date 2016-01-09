#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

/*
  http://stackoverflow.com/a/26939700/321897
  - sockaddr is the general form
  - sockaddr_in is the ipv4 form
  - sockaddr_in6 is the ipv6 form
  - sockaddr_un is the unix domain sockets form
*/
#include <sys/types.h>

struct sockaddr_in
gs_socket_address_init(uint16_t port, int a, int b, int c, int d) {
  uint32_t ip = ( a << 24 ) |
                ( b << 16 ) |
                ( c << 8  ) |
                ( d       );

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(ip);

  return address;
}

bool
gs_socket_init(uint16_t port, int *socket_handle) {
  int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (handle <= 0) {
    // fprintf(stderr, "Failed to create socket\n");
    return false;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(handle, (const struct sockaddr*)&address, sizeof(struct sockaddr_in) ) < 0) {
    // fprintf(stderr, "Failed to bind socket\n");
    return false;
  }

  int non_blocking = 1;
  if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1) {
    // fprintf(stderr, "Failed to set non-blocking\n");
    return false;
  }

  *socket_handle = handle;
  return true;
}

bool
gs_socket_send(int socket_handle, const char *packet_data, int packet_size, struct sockaddr_in address) {
  bool result;

  int sent_bytes = sendto(socket_handle,
                          (const char*)packet_data,
                          packet_size,
                          0,
                          (struct sockaddr*)&address,
                          sizeof(struct sockaddr_in));

  if (sent_bytes != packet_size) {
    // fprintf(stderr, "Failed to send packet\n");
    return false;
  }

  return true;
}

int
main(int argc, char **argv) {
  /* Start Send Packets */
  bool result;

  int socket_handle;
  result = gs_socket_init(30000, &socket_handle);
  if (!result) {
    fprintf(stderr, "Failed to create socket\n");
    return(1);
  }

  const char* packet_data = "Hello there!";
  int packet_size = strlen(packet_data);

  struct sockaddr_in address = gs_socket_address_init(30000, 127, 0, 0, 1);
  result = gs_socket_send(socket_handle, packet_data, packet_size, address);
  /* End Send Packets */

  /* Start Receive Packets */
  while(true) {
    uint8_t packet_data[256];
    uint32_t max_packet_size = sizeof(packet_data);

    struct sockaddr_in from;
    socklen_t from_length = sizeof(from);

    memset((void*)packet_data, 0, max_packet_size);
    int bytes = recvfrom(socket_handle, (char*)packet_data, max_packet_size, 0, (struct sockaddr*)&from, &from_length);
    if (bytes <= 0) break;

    uint32_t from_address = ntohl(from.sin_addr.s_addr);
    uint32_t from_port = ntohs(from.sin_port);

    fprintf(stdout, "Recevied data: port:%d, address: %d, data: %s\n", from_port, from_address, packet_data);
  }
  /* End Receive Packets */

  return 0;
}
