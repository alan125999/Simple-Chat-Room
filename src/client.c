#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "socket_handler.h"
#include "chatting_handler.h"


char *IP = "127.0.0.1";
int PORT = 23333;

int main(int argc, char *argv[]){
    int fd_connect;
    fd_connect = connect_server(IP, PORT);
    if(fd_connect < 0) return -1;

    client(fd_connect);
    close(fd_connect);
    return 0;
}