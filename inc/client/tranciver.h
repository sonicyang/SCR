#ifndef __TRANCIVER_H__
#define __TRANCIVER_H__

#include <netinet/in.h>
#include <pthread.h>

#include "list.h"
#include "tui.h"

struct client_tranciver_t{
    struct sockaddr_in server_address;
    int socket;
    pthread_t transmitter_thread_id;
    pthread_t reciver_thread_id;
    struct list_t* message_to_send;
    struct TUI_t* tui;
    char name[64];
    sem_t* sent;
};


void start_tranciver(struct client_tranciver_t*);
void connect_server(struct TUI_t*, char*, void*);
void input_handler(struct TUI_t* tui, char* input, void* argument);


#endif//__TRANCIVER_H__
