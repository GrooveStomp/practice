#ifndef GS_SOCKET_C
#define GS_SOCKET_C

#include <stdint.h>
#include <stdio.h>
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

typedef enum {
  GS_SOCKET_ERR_NONE,
  GS_SOCKET_ERR_CREATE,
  GS_SOCKET_ERR_BIND,
  GS_SOCKET_ERR_NON_BLOCKING,
  GS_SOCKET_ERR_SEND
} gs_socket_result;

//------------------------------------------------------------------------------------------------------------
// API Declarations
//------------------------------------------------------------------------------------------------------------
bool
gs_socket_success(gs_socket_result result);

bool
gs_socket_error(gs_socket_result result);

const char *
gs_socket_message(gs_socket_result result);

struct sockaddr_in
gs_socket_address_init(uint16_t port, int a, int b, int c, int d);

gs_socket_result
gs_socket_init(uint16_t port, int *socket_handle);

bool
gs_socket_send(int socket_handle, const char *packet_data, int packet_size, struct sockaddr_in address);

//------------------------------------------------------------------------------------------------------------
// API Implementations
//------------------------------------------------------------------------------------------------------------
bool
gs_socket_success(gs_socket_result result) {
  return GS_SOCKET_ERR_NONE == result;
}

bool
gs_socket_error(gs_socket_result result) {
  return GS_SOCKET_ERR_NONE != result;
}

const char *
gs_socket_message(gs_socket_result result) {
  switch(result) {
    case GS_SOCKET_ERR_NONE: {
      return "No error!";
      break;
    }
    case GS_SOCKET_ERR_CREATE: {
      return "Failed to create socket";
      break;
    }
    case GS_SOCKET_ERR_BIND: {
      return "Failed to bind socket";
      break;
    }
    case GS_SOCKET_ERR_NON_BLOCKING: {
      return "Failed to make socket non-blocking";
      break;
    }
    case GS_SOCKET_ERR_SEND: {
      return "Failed to send message";
      break;
    }
  }

  return "\0";
}

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

gs_socket_result
gs_socket_init(uint16_t port, int *socket_handle) {
  int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (handle <= 0) { return GS_SOCKET_ERR_CREATE; }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(handle, (const struct sockaddr*)&address, sizeof(struct sockaddr_in) ) < 0) {
    return GS_SOCKET_ERR_BIND;
  }

  int non_blocking = 1;
  if (fcntl(handle, F_SETFL, O_NONBLOCK, non_blocking) == -1) {
    return GS_SOCKET_ERR_NON_BLOCKING;
  }

  *socket_handle = handle;
  return GS_SOCKET_ERR_NONE;
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
    return GS_SOCKET_ERR_SEND;
  }

  return GS_SOCKET_ERR_NONE;
}

#endif // GS_SOCKET_C
