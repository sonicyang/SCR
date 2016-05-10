#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "message.h"
#include "misc.h"

void init_message(struct message_t* message, char* sender, int size){
    memset(message->sender, 0, sizeof(message->sender));
    strncpy(message->sender, sender, 63);
    message->timestamp = (int)time(NULL);
    message->buffer = malloc(size + 1);
    message->size = size;

    memset(message->buffer, 0, size + 1);

}

void destroy_message(struct message_t* message){
    free(message->buffer);
}

void send_message(struct message_t* message, int* socket){
    int n;

    n = write(*socket, &message->timestamp, sizeof(int));
    n = write(*socket, message->sender, sizeof(message->sender));
    n = write(*socket, message->buffer, message->size);
    if(n != message->size)
        die("Data lose detected");
}

void recv_message(struct message_t* message, int* socket){
    int n;

    n = read(*socket , &message->timestamp, sizeof(int));
    n = read(*socket , message->sender, sizeof(message->sender));
    n = read(*socket , message->buffer, message->size);
    if(n != message->size)
        die("Data lose detected");
}
