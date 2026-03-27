#ifndef PAC_SOCKET_H
#define PAC_SOCKET_H

#include <sys/un.h>

typedef struct {
  int server_fd;
  struct sockaddr_un addr;
} un_socket_t;

int socket_create(un_socket_t* sock, const char* path);
void socket_destroy(un_socket_t* sock);

#endif
