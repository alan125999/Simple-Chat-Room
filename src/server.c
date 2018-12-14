#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#include "socket_handler.h"
#include "chatting_handler.h"
#include "user.h"

int PORT = 23333;

void *thread(void *arg);

struct Thread_args {
    int fd_accept, *user_num;
    struct User *user_list;
    short reject;
};

int main(int argc, char *argv[]) {
    // Variable Declarations
    int listen_fd, fd_accept, ret, user_num;
    pthread_t tid;
    struct Thread_args *args;
    struct User user_list[MAX_USERS];

    // Initail User_list
    user_num = 0;
    bzero(user_list, sizeof(user_list));

    // Create listen socket
    listen_fd = create_listen_socket(PORT);
    if(listen_fd < 0) return -1;

    // Accept Client
    for( ; ; ){
        fd_accept = accept_client_socket(listen_fd);
		if (fd_accept < 0) continue;
		
		// Pack up Thread Args
        args = malloc(sizeof(struct Thread_args));
        if(args == NULL) {
            perror("malloc Thread_args");
            continue;
        }
        if(user_num++ >= MAX_USERS) args->reject = 1;
        else args->reject = 0;
        args->fd_accept = fd_accept;
        args->user_list = user_list;
        args->user_num = &user_num;

        // Create Thread
        ret = pthread_create(&tid, NULL, thread, (void *)args);
        if(ret != 0) {
            perror("Create thread");
            continue;
        }
    }
    return 0;
}

void *thread(void *thread_args){
    struct Thread_args *args = thread_args;
    if(args->reject) reject_client(args->fd_accept);
    else serve_client(args->fd_accept, args->user_list);
    close(args->fd_accept);
    *(args->user_num) -= 1;
    free(args);
    return NULL;
}