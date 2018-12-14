#ifndef CHATTING_HANDLER_H
#define CHATTING_HANDLER_H

#include "user.h"

int reject_client(int fd_accept);
int serve_client(int fd_accept, struct User user_list[]);
int client(int fd_connect);
int client_set_name(int fd_connect);
int client_endless_recv(int fd_connect);
int serve_set_name(int fd_accept, struct User user_list[]);
void *client_endless_send(void *fd_connect);
int serve_send_all(char message[], int user, struct User user_list[]);
int serve_command(char buffer[], int user, struct User user_list[]);
int get_time(char *time_str);
int serve_send_msg(char msg[], int user, struct User user_list[]) ;
#endif