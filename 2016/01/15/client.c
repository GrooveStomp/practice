#define _SVID_SOURCE
#include <features.h>

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

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
  if ((socket_handle = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, close_socket) == SIG_ERR) {
    fprintf(stderr, "Failed to register 'close_socket' signal handler\n");
    exit(EXIT_FAILURE);
  }

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1) {
    fprintf(stderr, "Couldn't make socket non-blocking\n");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server;
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)30000);
  if (inet_aton("127.0.0.1", &server.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(EXIT_FAILURE);
  }

  const char* packet_data = "Hello there!";
  size_t packet_size = strlen(packet_data);

  printf("[Client] Sending data:\t\taddress: '%u', port: '%u', message: '%s'\n", ntohl(server.sin_addr.s_addr), ntohs(server.sin_port), packet_data);

  if (sendto(socket_handle, packet_data, packet_size, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) != packet_size) {
    fprintf(stderr, "Failed to send datagram\n");
    exit(EXIT_FAILURE);
  }

  close(socket_handle);
  exit(EXIT_SUCCESS);
}
