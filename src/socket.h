#ifndef PAC_SOCKET_H
#define PAC_SOCKET_H

#include <stdio.h>
#include <sys/un.h>

typedef struct {
  int server_fd;
  int client_fd;
  struct sockaddr_un addr;
} un_socket_t;

int socket_create(un_socket_t* sock, const char* path);
void socket_destroy(un_socket_t* sock);
void socket_accept(un_socket_t* sock);
ssize_t socket_read(un_socket_t* sock, char* buffer, size_t buffer_length);

#endif
