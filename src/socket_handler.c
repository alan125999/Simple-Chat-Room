#include <stdio.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_handler.h"

int create_listen_socket(int port) {
    int fd_listen, ret;
    struct sockaddr_in addr_server;

    // Create socket
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen == -1){
        perror("Create socket");
        return -1;
    }

    // Set Addr
    addr_server.sin_family = PF_INET;
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_port = htons(port);

    // bind
    bzero(&addr_server, sizeof(addr_server));
    ret = bind(fd_listen, (struct sockaddr *) &addr_server, sizeof(addr_server));
    if(ret < 0){
		perror("bind");
		return -1;
	}

    // listen
    ret = listen(fd_listen, MAX_CONN);
	if(ret < 0){
		perror("listen");
		return -1;
	}
    return fd_listen;
}

int accept_client_socket(int fd_listen){
    struct sockaddr_in addr_client;
    size_t addrlen;
    int fd_accept;
    
    addrlen= sizeof(addr_client);
    fd_accept = accept(fd_listen, (struct sockaddr *) &addr_client, &addrlen);
    if(fd_accept < 0){
		perror("accept");
		return -1;
	}
    return fd_accept;
}