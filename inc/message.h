#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <sys/socket.h>
#include <pthread.h>

#include "list.h"
#include "setting.h"

#define MAX_NAME 15

extern struct list_t* message_list;

struct message_t{
    int timestamp;
    char sender[MAX_NAME + 1];
    char* buffer;
    int size;
};

void init_message(struct message_t*, char*, int);
void destroy_message(struct message_t*);
void send_message(struct message_t*, int*);
void recv_message(struct message_t*, int*);

#endif//__MESSAGE_H__
