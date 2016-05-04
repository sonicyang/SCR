#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#include "setting.h"
#include "pool.h"
#include "misc.h"

struct client_t{
    struct sockaddr_in address;
    socklen_t length;
    int socket;
    pthread_t reciver_thread_id;
    pthread_t transmitter_thread_id;
    struct list_t* sem_list;
    sem_t* sem;
    int activate;
};

void client_clean_up(void* argument);
void client_receiver(struct client_t* argument);
void client_transmitter(struct client_t* argument);
int start_client_handler(struct client_t* arg);

#endif//__CLIENT_HANDLER_H__
