#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message.h"
#include "misc.h"

void init_message(struct message_t* message, char* sender, int size){
    message->sender = sender;
    message->timestamp = (int)time(NULL);
    message->buffer = malloc(size + 1);
    memset(message->buffer, 0, size + 1);

    message->size = size;
}

void destroy_message(struct message_t* message){
    free(message->buffer);
}

void send_message(struct message_t* message, int* socket){
    int n;

    n = write(*socket, message, message->size);
    if(n != message->size)
        die("Data lose detected");
}

void recv_message(struct message_t* message, int* socket){
    int n;

    n = read(*socket , message->buffer, message->size);
    if(n != message->size)
        die("Data lose detected");
}
