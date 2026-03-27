#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

int socket_create(un_socket_t* sock, const char* path) {
  int server_fd;
  struct sockaddr_un addr;

  if (strlen(path) >= sizeof(addr.sun_path)) {
    perror("Path is too long");
    return -1;
  }

  if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("Unable to create socket");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
  addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

  unlink(path);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("Error binding socket");
    goto cleanup;
  }

  if (listen(server_fd, 5) == -1) {
    perror("Error listening on socket");
    goto cleanup;
  }

  // set non blocking
  int curr_flags = fcntl(server_fd, F_GETFL, 0);
  if (curr_flags == -1) {
    perror("Error gettings flags");
    goto cleanup;
  }

  if (fcntl(server_fd, F_SETFL, curr_flags | O_NONBLOCK) == -1) {
    perror("Error setting flags");
    goto cleanup;
  };

  sock->server_fd = server_fd;
  sock->addr = addr;
  return 0;

cleanup:
  close(server_fd);
  memset(sock, 0, sizeof(un_socket_t));
  return -1;
}

void socket_destroy(un_socket_t* sock) {
  if (sock == NULL || sock->server_fd < 0) {
    return;
  }

  close(sock->server_fd);
  unlink(sock->addr.sun_path);
  sock->server_fd = -1;
}
