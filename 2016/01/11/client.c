#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

int
main(int argc, char **argv) {

  int socket_handle;
  if ((socket_handle = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "Failed to create socket\n");
    return 1;
  }

  const char *client_path = "/tmp/udp_sock_client";
  struct sockaddr_un client_address;
  memset(&client_address, 0, sizeof(struct sockaddr_un));
  client_address.sun_family = AF_UNIX;
  strncpy(client_address.sun_path, client_path, strlen(client_path));
  if (bind(socket_handle, (struct sockaddr *)&client_address, sizeof(struct sockaddr_un)) == -1) {
    fprintf(stderr, "Failed to bind socket\n");
    return 1;
  }

  const char *server_path = "/tmp/udp_sock_server";
  struct sockaddr_un server_address;
  memset(&server_address, 0, sizeof(struct sockaddr_un));
  server_address.sun_family = AF_UNIX;
  strncpy(server_address.sun_path, server_path, strlen(server_path));

  if (fcntl(socket_handle, F_SETFL, O_NONBLOCK, 1) == -1) {
    fprintf(stderr, "Couldn't make socket non-blocking\n");
    return 1;
  }

  const char* packet_data = "Hello there!";
  size_t packet_size = strlen(packet_data);

  if (sendto(socket_handle, packet_data, packet_size, 0, (struct sockaddr *)&server_address,
             sizeof(struct sockaddr_un)) != packet_size) {
    fprintf(stderr, "Failed to send datagram\n");
    return 1;
  }

  if (remove(client_path) == -1 && errno != ENOENT) {
    fprintf(stderr, "Failed to remove file: %s\n", client_path);
    return 1;
  }

  return 0;
}
