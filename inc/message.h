#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <sys/socket.h>
#include <pthread.h>

#include "setting.h"

extern struct pool_t* message_pool;

struct message_t{
    int timestamp;
    char* sender;
    char* buffer;
    int size;
};

void init_message(struct message_t*, char*, int);
void destroy_message(struct message_t*);
void send_message(struct message_t*, int*);
void recv_message(struct message_t*, int*);

#endif//__MESSAGE_H__
