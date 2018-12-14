#include <stdlib.h>
#include <string.h>

#include "user.h"

int create_user(struct User user_list[], char *name, int socket) {
    int i, target;
    target = -1;
    for(i = 0; i < MAX_USERS; i++) {
        // Find First Empty Space
        if(user_list[i].online == 0 && target == -1) {
            target = i;
        }
        // Check Duplication
        if(user_list[i].online == 1 && strcmp(user_list[i].name, name) == 0){
            return -1;
        }
    }
    if(target == -1) return -2;
    user_list[target].online = 1;
    strcpy(user_list[target].name, name);
    user_list[target].socket = socket;
    pthread_mutex_init(&(user_list[target].lock), NULL);
    return target;
}

void delete_user(struct User user_list[], int index) {
    user_list[index].online = 0;
}