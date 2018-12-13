#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>

#include "socket_handler.h"

int PORT = 23333;

// Prototypes
void *serve_client(void *fd_accept);

int main(int argc, char *argv[]) {
    // Variable Declarations
    int listen_fd, fd_accept, ret;
    pthread_t tid;

    // Create listen socket
    listen_fd = create_listen_socket(PORT);
    if(listen_fd < 0) return -1;

    // Accept Client
    for( ; ; ){
        fd_accept = accept_client_socket(listen_fd);
		if (fd_accept < 0) continue;
		
		// Create Thread
        ret = pthread_create(&tid, NULL, serve_client, (void *)fd_accept);
        if(ret != 0) {
            perror("Create thread");
            continue;
        }
    }
    return 0;
}

void *serve_client(void *fd_accept){
    int fd = (int)fd_accept;
    static char *welcome_message = "Welcome\n";
    send(fd_accept, welcome_message, strlen(welcome_message), 0);
}