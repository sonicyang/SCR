#include <unistd.h>
#include <stdio.h>

#include "client_handler.h"
#include "misc.h"

void client_clean_up(void* argument){
    struct client_t* tmp = (struct client_t*) argument;
    close(tmp->socket);
}

void client_handler(struct client_t* argument){
    char buffer[256];
    int byte_read;

    pthread_cleanup_push(client_clean_up, argument);

    byte_read = read(argument->socket ,buffer, 255);

    if (byte_read < 0)
        die("die reading from socket");

    printf("Here is the message: %s\n",buffer);

    /*n = write(newsockfd,"I got your message",18);*/
    /*if (n < 0) die("die writing to socket");*/

    pthread_cleanup_pop(1);
    return;
}

int start_client_handler(struct client_t* arg){
    if(pthread_create(&(arg->thread_id), NULL, (void*)client_handler, (void*)arg)){
        die("Failed on creating Client Handler thread");
    }
    return 0;
}
