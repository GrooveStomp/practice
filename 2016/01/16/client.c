#define _SVID_SOURCE
#include <features.h> /* To get inet_aton() */

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>

#include "common.c"

int socket_handle;

void
close_socket(int signo) {
  if (close(socket_handle) == -1)
    fprintf(stderr, "Failed to close socket\n");
  raise(signo);
}

int
main(int argc, char **argv) {
  if ((socket_handle = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    gsnet_error("Failed to create socket\n");

  if (signal(SIGINT, close_socket) == SIG_ERR)
    gsnet_error("Failed to register 'close_socket', signal handler\n");

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1)
    gsnet_error("Couldn't make socket non-blocking\n");

  struct sockaddr_in server;
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;

  char text[GSNET_PACKET_DATA_SIZE];
  printf("UDP Socket Client Sample Program\n");
  printf("--------------------------------\n");

  printf("[Client] Enter Server IPV4 address: ");
  memset(text, 0, GSNET_PACKET_DATA_SIZE);
  if (NULL == gs_gets(text, GSNET_PACKET_DATA_SIZE))
    gsnet_error("Couldn't read server IP\n");
  if (inet_aton(text, &server.sin_addr) == 0)
    gsnet_error("inet_aton() failed\n");

  printf("[Client] Enter Server Port: ");
  memset(text, 0, GSNET_PACKET_DATA_SIZE);
  if (NULL == gs_gets(text, GSNET_PACKET_DATA_SIZE))
    gsnet_error("Couldn't read server port\n");
  server.sin_port = htons((uint16_t)atoi(text));

  struct gsnet_connection server_connection;
  server_connection.address = server.sin_addr.s_addr;
  server_connection.port = ntohs(server.sin_port);

  char packet[GSNET_PACKET_MAX_SIZE];
  memset(packet, 0, GSNET_PACKET_MAX_SIZE);
  gsnet_init_packet(packet);

  printf("[Client] Enter your data (Ctrl+d or Ctrl+c to stop):\n");

  while(true) {
    memset((char *)gsnet_get_packet_data(packet), 0, GSNET_PACKET_DATA_SIZE);
    if (fgets(text, GSNET_PACKET_DATA_SIZE, stdin) == NULL)
      break;

    char *newline = memchr(text, '\n', GSNET_PACKET_DATA_SIZE);
    if (newline != NULL)
      *newline = '\0';

    gsnet_set_packet_data(packet, text, strlen(text));

    printf("[Client] Sending: '%s'\n", text);
    if (sendto(socket_handle, packet, sizeof(packet), 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) != sizeof(packet))
      gsnet_error("Failed to send datagram\n");

    /* while(true) { */
    /*   while(true) { */
    /*     memset((void*)packet, 0, GSNET_PACKET_MAX_SIZE); */

    /*     struct sockaddr_in from; */
    /*     socklen_t from_length = sizeof(from); */
    /*     if (recvfrom(socket_handle, (char*)packet, GSNET_PACKET_MAX_SIZE, 0, (struct sockaddr *)&from, &from_length) <= 0) */
    /*       break; */

    /*     int from_address = ntohl(from.sin_addr.s_addr); */
    /*     int from_port = ntohs(from.sin_port); */

    /*     if (memcmp(&server_connection, &gsnet_null_connection, sizeof(struct connection)) == 0) { */
    /*       server.address = from_address; */
    /*       server.port = from_port; */
    /*       fprintf(stdout, "[Server] Connection initialized with IP: '%s' on port: '%d', message: '%s'\n", */
    /*               inet_ntoa(from.sin_addr), */
    /*               from_port, */
    /*               gsnet_get_packet_data(packet)); */
    /*     } */
    /*     else if (from_address == server.address && from_port == server.port) { */
    /*       fprintf(stdout, "[Server] Message received: '%s'\n", gsnet_get_packet_data(packet)); */
    /*     } */
    /*     else { */
    /*       fprintf(stdout, "[Server] Filtered out message from IP:   '%s' on port: '%d'\n", inet_ntoa(from.sin_addr), from_port); */
    /*     } */
    /*   } */
    /* } */
  }

  close(socket_handle);
  exit(EXIT_SUCCESS);
}
