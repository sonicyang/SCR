#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <sys/socket.h>
#include <pthread.h>

#include "setting.h"
#include "misc.h"

struct client_t{
    struct sockaddr_in address;
    socklen_t length;
    int socket;
    pthread_t thread_id;
    int activate;
};

struct message_t{
    int timestamp;
    char* sender;
    char* buffer;
};

void client_clean_up(void* argument);
void client_handler(struct client_t* argument);
int start_client_handler(struct client_t* arg);


#endif//__CLIENT_HANDLER_H__
