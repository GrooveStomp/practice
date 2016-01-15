#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "common.c"

int socket_handle;

void
close_socket(int signo) {
  if (close(socket_handle) == -1) {
    fprintf(stderr, "Failed to close socket\n");
  }
  raise(signo);
}

int
main(int argc, char **argv) {
  struct sockaddr_in client_address;
  memset(&client_address, 0, sizeof(struct sockaddr_in));
  client_address.sin_family = AF_INET;
  client_address.sin_addr.s_addr = INADDR_ANY;
  client_address.sin_port = htons((uint16_t)30000);

  if ((socket_handle = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, close_socket) == SIG_ERR) {
    fprintf(stderr, "Failed to register 'close_socket' signal handler\n");
    exit(EXIT_FAILURE);
  }

  if (bind(socket_handle, (struct sockaddr *)&client_address, sizeof(struct sockaddr_in)) == -1) {
    fprintf(stderr, "Failed to bind socket: %s\n", socket_bind_error_string(errno));
    exit(EXIT_FAILURE);
  }

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1) {
    fprintf(stderr, "Couldn't make socket non-blocking\n");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(struct sockaddr_in));
  server_address.sin_family = AF_INET;
  uint32_t server_address_ipv4 =
    (uint32_t)192 << 24 |
    (uint32_t)168 << 16 |
    (uint32_t)  1 <<  8 |
    (uint32_t)125;
  printf("addr: %u\n", server_address_ipv4);
  server_address.sin_addr.s_addr = htonl(server_address_ipv4);
  server_address.sin_port = htons((uint16_t)30000);

  const char* packet_data = "Hello there!";
  size_t packet_size = strlen(packet_data);

  printf("Sending message: '%s' to address: '%u' on port: '%u'\n", packet_data, ntohl(server_address.sin_addr.s_addr), ntohs(server_address.sin_port));

  if (sendto(socket_handle, packet_data, packet_size, 0, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) != packet_size) {
    fprintf(stderr, "Failed to send datagram\n");
    exit(EXIT_FAILURE);
  }

  close(socket_handle);
  exit(EXIT_SUCCESS);
}
