#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#include "chatting_handler.h"
#include "user.h"

#define BUFFER_SIZE 1000
#define TIME_STR_SIZE 40

int reject_client(int fd_accept){
    char reject_message[] = "REJ";
    send(fd_accept, reject_message, strlen(reject_message), 0);
    return 0;
}

int serve_client(int fd_accept, struct User user_list[]){
    int ret, user_index, len, i;
    char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE], time_str[TIME_STR_SIZE];

    user_index = serve_set_name(fd_accept, user_list);
    if(user_index < 0) return -1;

    get_time(time_str);
    sprintf(buffer2, "[ SERVER @ %s ]\n%s joined the room!\n\n", time_str, user_list[user_index].name);
    serve_send_all(buffer2, user_index, user_list);

    for( ; ; ){
        ret = read(fd_accept, buffer, BUFFER_SIZE);
        if(ret < 1) {
            if(ret < 0) perror("read");
            break;
        }
        buffer[ret] = '\0'; 
        len = strlen(buffer);
        for(i = 0; i < len; i++){
            if(buffer[i] != ' ') break;
        }
        if(buffer[i] == '/') {
            serve_command(buffer, user_index, user_list);
        }
        else{
            get_time(time_str);
            sprintf(buffer2, "[ %s @ %s ]\n", user_list[user_index].name, time_str);
            strncat(buffer2, buffer, strlen(buffer));
            strncat(buffer2, "\n", strlen("\n"));
            serve_send_all(buffer2, user_index, user_list);
        }
        
    }

    printf("index: %d, name: %s, state: offline\n", user_index, user_list[user_index].name);
    delete_user(user_list, user_index);
    return 0;
}

int serve_set_name(int fd_accept, struct User user_list[]){
    int ret, user_index;
    char name_dup_message[] = "DUP";
    char unknown_message[] = "UNKNOWN";
    char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    
    for( ; ; ){
        ret = read(fd_accept, buffer, BUFFER_SIZE);
        if(ret < 0) {
            perror("read");
            return -1;
        }
        user_index = create_user(user_list, buffer, fd_accept);
        if(user_index >= 0) break;
        if(user_index == -1) send(fd_accept, name_dup_message, strlen(name_dup_message), 0);
        if(user_index == -2) send(fd_accept, unknown_message, strlen(unknown_message), 0);
    }
    sprintf(buffer2, "\033[2J\033[HWelcome %s\n", buffer);
    ret = serve_send_msg(buffer2, user_index, user_list);
    if(ret < 0) {
        fprintf(stderr, "serve_set_name send error\n");
        return -2;
    }
    printf("index: %d, name: %s, state: online\n", user_index, buffer);
    return user_index;
}

int serve_send_all(char message[], int user, struct User user_list[]) {
    int i, ret;
    for(i = 0; i < MAX_USERS; i++){
        if( i == user) continue;
        if(user_list[i].online){
            ret = serve_send_msg(message, i, user_list);
            if(ret < 0) {
                fprintf(stderr, "Endless Send error\n");
                continue;
            }
        }
    }
    return 0;
}
int serve_command(char buffer[], int user, struct User user_list[]){
    int ret, i, flag, target;
    char *token, *next, buf[BUFFER_SIZE], time_str[TIME_STR_SIZE];
    char help_msg[] = 
        "-- Commands --\n"
        "/help: Show this message\n"
        "/users: Show online users\n"
        "/msg <user> <message>: Send private message to specific user\n"
        "/send <user> <file_path>: Send file to specific user\n"
        "/abort <user>: Abort to send file to specific user\n"
        "/recv <user> <file_path>: Receive file from specific user\n"
        "/reject <user>: Reject to receive file";
    char unknown_msg[] = 
        "Undefined command\n\n";
    char users_msg[] = 
        "-- Online Users --\n";

    token = strtok_r(buffer, " \n", &next);
    if(strcmp(token, "/help") == 0){
        ret = serve_send_msg(help_msg, user, user_list);
        if(ret < 0) {
            fprintf(stderr, "serve_command help error\n");
            return -1;
        }
    }
    else if(strcmp(token, "/users") == 0){
        strcpy(buf, users_msg);
        flag = 0;
        for(i = 0; i < MAX_USERS; i++){
            if(user_list[i].online){
                if(flag) strncat(buf, ", ", strlen(", "));
                else flag = 1;
                strncat(buf, user_list[i].name, strlen(user_list[i].name));
            }
        }
        strncat(buf, "\n\n", strlen("\n\n"));
        ret = serve_send_msg(buf, user, user_list);
        if(ret < 0) {
            fprintf(stderr, "serve_command users error\n");
            return -1;
        }
    }
    else if(strcmp(token, "/msg") == 0){
        token = strtok_r(next, " \n", &next);
        if(token == NULL) {
            ret = serve_send_msg("Usage: /msg <user> <message>\n\n", user, user_list);
            return 0;
        }
        target = -1;
        for(i = 0; i < MAX_USERS; i++){
            if(user_list[i].online && strcmp(user_list[i].name, token) == 0){
                target = i;
                break;
            }
        }
        if(target < 0) {
            ret = serve_send_msg("No such user.\n\n", user, user_list);
            if(ret < 0) {
                fprintf(stderr, "serve_command msg error\n");
                return -1;
            }
        }
        else{
            get_time(time_str);
            sprintf(buf, "[ %s -PM-> %s @ %s ]\n", user_list[user].name, token, time_str);
            strncat(buf, next, strlen(next));
            strncat(buf, "\n", strlen("\n"));
            ret = serve_send_msg(buf, user, user_list);
            if(ret < 0) {
                fprintf(stderr, "serve_command msg error\n");
                return -1;
            }
            ret = serve_send_msg(buf, i, user_list);
            if(ret < 0) {
                fprintf(stderr, "serve_command msg error\n");
                return -1;
            }
        }
    }
    else{
        ret = serve_send_msg(unknown_msg, user, user_list);
        if(ret < 0) {
            fprintf(stderr, "serve_command unknown error\n");
            return -1;
        }
    }
    return 0;
}

int get_time(char *time_str) {
    time_t rawtime;
    struct tm * timeinfo;
    int len;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strcpy(time_str, asctime(timeinfo));
    len = strlen(time_str);
    time_str[len - 1] = '\0';
    return 0;
}

int client(int fd_connect) {
    int ret;
    pthread_t tid;
    ret = client_set_name(fd_connect);
    if(ret < 0) return -1;
    // Create Thread
    ret = pthread_create(&tid, NULL, client_endless_send, (void *)fd_connect);
    if(ret != 0) {
        perror("Create thread");
        return -1;
    }
    // Endless Recv
    ret = client_endless_recv(fd_connect);
    pthread_kill(tid, SIGINT);
    return 0;
}

int client_set_name(int fd_connect){
    int ret, len;
    char buffer[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    for( ; ; ){
        printf("Username: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        len = strlen(buffer);
        if(buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len -= 1;
        }
        if(len > USERNAME_SIZE - 1) {
            fprintf(stderr, "Username must be shorter than %d charachters.\n", USERNAME_SIZE - 1);
            continue;
        }
        send(fd_connect, buffer, strlen(buffer), 0);
        ret = read(fd_connect, buffer, BUFFER_SIZE);
        if(ret < 1) {
            if(ret < 0) perror("read");
            else printf("Connection Closed.");
            return -1;
        }
        buffer[ret] = '\0';
        sscanf(buffer, "%s", buffer2);
        if(strcmp(buffer2, "DUP") == 0) {
            puts("Username duplicated, please use another one.");
            continue;
        }
        if(strcmp(buffer2, "UNKNOWN") == 0) {
            puts("Unknown error, please retry later."); 
            return -2;
        }
        if(strcmp(buffer2, "REJ") == 0) {
            puts("Too many users online. Please retry later."); 
            return -3;
        }
        break;
    }
    printf("%s", buffer);
    
    return 0;
}

int client_endless_recv(int fd_connect){
    int ret;
    char buffer[BUFFER_SIZE];

    for( ; ; ){
        ret = read(fd_connect, buffer, BUFFER_SIZE);
        if(ret < 1) {
            if(ret < 0) perror("read");
            else printf("Connection Closed\n");
            return -1;
        }
        buffer[ret] = '\0';
        printf("%s", buffer);
    }
    return 0;
}

void *client_endless_send(void *fd_connect){
    int ret, fd;
    char buffer[BUFFER_SIZE];
    fd = (int)fd_connect;
    for( ; ; ){
        if(fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("Endless Send Fgets");
            return NULL;
        }
        
        ret = send(fd, buffer, strlen(buffer), 0);
        if(ret < 0){
            perror("Endless Send");
            return NULL;
        }
    }
    return NULL;
}

int serve_send_msg(char msg[], int user, struct User user_list[]) {
    int ret;
    pthread_mutex_lock(&(user_list[user].lock));
    ret = send(user_list[user].socket, msg, strlen(msg), 0);
    pthread_mutex_unlock(&(user_list[user].lock));
    if(ret < 0){
        perror("serve_send_msg");
        return -1;
    }
    return 0;
}