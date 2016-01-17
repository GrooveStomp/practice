#include <stdint.h>
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
#include <time.h>

#include "common.c"

#define CONNECTION_TIMEOUT_S 10
int socket_handle;

void
close_socket(int signo) {
  if (close(socket_handle) == -1)
    fprintf(stderr, "Failed to close socket\n");
  raise(signo);
}

int
main(int argc, char **argv) {

  printf("+------------------------------------------------------------------------------+\n");
  printf("|                             UDP Socket Server                                |\n");
  printf("+------------------------------------------------------------------------------+\n");
  uint8_t packet[GSNET_PACKET_MAX_SIZE];
  struct gsnet_connection client = { 0 };

  struct sockaddr_in address;
  memset(&address, 0, sizeof(struct sockaddr_in));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons((uint16_t)30000);

  if ((socket_handle = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, close_socket) == SIG_ERR) {
    fprintf(stderr, "Failed to register 'close_socket' signal handler\n");
    exit(EXIT_FAILURE);
  }

  if (bind(socket_handle, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1) {
    fprintf(stderr, "Failed to bind socket\n");
    exit(EXIT_FAILURE);
  }

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1) {
    fprintf(stderr, "Couldn't make socket non-blocking\n");
    exit(EXIT_FAILURE);
  }

  time_t last_time, curr_time;
  if ((last_time = time(NULL)) == -1)
    gsnet_error("time() returned -1\n");
  double time_delta_s = 0;

  while(1) {
    if ((curr_time = time(NULL)) == -1)
      gsnet_error("time() returned -1\n");
    time_delta_s = difftime(curr_time, last_time);

    if (memcmp(&client, &gsnet_null_connection, sizeof(struct gsnet_connection)) != 0 &&
        (client.elapsed_time_seconds = time_delta_s) > CONNECTION_TIMEOUT_S) {
      printf("[Server] Lost connection with client. Timed out.\n");
      client.address = client.port = client.elapsed_time_seconds = 0;
    }

    memset((void*)packet, 0, GSNET_PACKET_MAX_SIZE);

    struct sockaddr_in from;
    socklen_t from_length = sizeof(from);
    if (recvfrom(socket_handle, (char*)packet, GSNET_PACKET_MAX_SIZE, 0, (struct sockaddr *)&from, &from_length) <= 0)
      continue;

    int from_address = ntohl(from.sin_addr.s_addr);
    int from_port = ntohs(from.sin_port);

    if (memcmp(&client, &gsnet_null_connection, sizeof(struct gsnet_connection)) == 0) {
      client.address = from_address;
      client.port = from_port;
      fprintf(stdout, "[Server] Connection initialized with IP: '%s' on port: '%d', message: '%s'\n",
              inet_ntoa(from.sin_addr),
              from_port,
              gsnet_get_packet_data(packet));
      client.elapsed_time_seconds = 0;
      last_time = curr_time;
    }
    else if (from_address == client.address && from_port == client.port) {
      fprintf(stdout, "[Server] Message received: '%s'\n", gsnet_get_packet_data(packet));
      last_time = curr_time;
    }
    else {
      fprintf(stdout, "[Server] Filtered out message from IP:   '%s' on port: '%d'\n", inet_ntoa(from.sin_addr), from_port);
    }
  }

  close(socket_handle);
  exit(EXIT_SUCCESS);
}
