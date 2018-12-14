#ifndef USER_H
#define USER_H

#define USERNAME_SIZE 11
#define MAX_USERS 10

#include <pthread.h>

struct User {
    char name[USERNAME_SIZE];
    int socket;
    int online;
    pthread_mutex_t lock;
};


int create_user(struct User user_list[], char *name, int socket);
void delete_user(struct User user_list[], int index);

#endif