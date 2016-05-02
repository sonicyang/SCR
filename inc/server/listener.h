#ifndef __LISTENER_H__
#define __LISTENER_H__

#include <sys/socket.h>
#include <pthread.h>

#include "setting.h"

struct listener_t{
    struct sockaddr_in address;
    int socket;
    pthread_t thread_id;
};

int start_listener(struct setting_t* setting, pthread_t* listener_id);
void listener_clean_up(void* argument);
void listener(struct setting_t* setting);


#endif//__LISTENER_H__
