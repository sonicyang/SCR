#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_handler.h"
#include "misc.h"

void client_clean_up(void* argument){
    struct client_t* tmp = (struct client_t*) argument;
    close(tmp->socket);
    tmp->activate = -1;
}

void client_handler(struct client_t* argument){
    char* buffer;
    struct packet_t packet;
    int byte_read;
    int run = 1;

    pthread_cleanup_push(client_clean_up, argument);

    while(run){
        byte_read = read(argument->socket , &packet, sizeof(struct packet_t));

        if(byte_read < 0)
            die("Error on reading socket");

        if(byte_read < sizeof(struct packet_t))
            die("Got non-standard packet");

        switch(packet.command){
            case MESG:
                buffer = malloc(packet.parameter + 1);
                memset(buffer, 0, packet.parameter);
                byte_read = read(argument->socket , buffer, packet.parameter);
                if(byte_read != packet.parameter)
                    die("Data lose detected");
                printf("Here is the message: %s\n",buffer);
                break;
            case TERM:
                run = 0;
                break;
            default:
                printf("%d %d", packet.command, TERM);
                die("Ambiguous command");
        }

        /*n = write(newsockfd,"I got your message",18);*/
        /*if (n < 0) die("die writing to socket");*/

    }


    pthread_cleanup_pop(1);
    return;
}

int start_client_handler(struct client_t* arg){
    if(pthread_create(&(arg->thread_id), NULL, (void*)client_handler, (void*)arg)){
        die("Failed on creating Client Handler thread");
    }
    return 0;
}
