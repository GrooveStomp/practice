#include <errno.h>

const char *
socket_bind_error_string(int error) {
  switch(error) {
    case(EACCES): {
      return "The address is protected, and the user is not the superuser.";
      break;
    }
    case(EADDRINUSE): {
      return "The given address is already in use.";
      break;
    }
    case(EBADF): {
      return "sockfd is not a valid descriptor.";
      break;
    }
    case(EINVAL): {
      return "The socket is already bound to an address.";
      break;
    }
    case(ENOTSOCK): {
      return "sockfd is a descriptor for a file, not a socket.";
      break;
    }
  }

  return "";
}
