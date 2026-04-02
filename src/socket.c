#include "socket.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

int socket_create(un_socket_t* sock, const char* path) {
  int server_fd;
  sock->client_fd = -1;
  sock->server_fd = -1;
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

  if (listen(server_fd, 1) == -1) {
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
  if (sock->client_fd >= 0) {
    close(sock->client_fd);
  }

  unlink(sock->addr.sun_path);
  sock->server_fd = -1;
  sock->client_fd = -1;
}

void socket_accept(un_socket_t* sock) {
  if (sock->client_fd > 0) {
    return;
  }

  int client_fd = accept(sock->server_fd, NULL, NULL);
  if (client_fd > 0) {
    int curr_flags = fcntl(client_fd, F_GETFL, 0);
    if (curr_flags != -1) {
      fcntl(client_fd, F_SETFL, curr_flags | O_NONBLOCK);
    }
    sock->client_fd = client_fd;
  }
}

ssize_t socket_read(un_socket_t* sock, char* buffer, size_t buffer_length) {
  if (sock->client_fd <= 0) {
    return 0;
  }

  ssize_t bytes_read = read(sock->client_fd, buffer, buffer_length);
  if (bytes_read > 0) {
    return bytes_read;
  } else if (bytes_read == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
  }

  close(sock->client_fd);
  sock->client_fd = -1;
  return 0;
}

ssize_t socket_write(un_socket_t* sock, char* buffer, size_t buffer_length) {
  if (sock->client_fd <= 0) {
    return 0;
  }

  ssize_t bytes_written = send(sock->client_fd, buffer, buffer_length, MSG_NOSIGNAL);
  if (bytes_written > 0) {
    return bytes_written;
  } else if (bytes_written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    return 0;
  }

  close(sock->client_fd);
  sock->client_fd = -1;
  return 0;
}
