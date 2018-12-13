#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#define MAX_CONN 100

int create_listen_socket(int port);
int accept_client_socket(int fd_listen);

#endif