#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "client_handler.h"
#include "packet.h"
#include "pool.h"
#include "message.h"
#include "misc.h"

void client_clean_up(void* argument){
    struct client_t* tmp = (struct client_t*) argument;
    close(tmp->socket);
    tmp->activate = -1;
}

void client_handler(struct client_t* argument){
    char name[64];
    struct packet_t packet;
    struct message_t* message;
    int last_unread_message = 0;
    int i;
    int run = 1;

    pthread_cleanup_push(client_clean_up, argument);

    strcpy(name, inet_ntoa(argument->address.sin_addr));

    while(run){
        wait_for_packet(&argument->socket, &packet);

        switch(packet.command){
            case MESG:
                message = pool_allocate(message_pool);
                init_message(message, name, packet.parameter);
                recv_message(message, &argument->socket);

                printf("Here is the message: %s\n", message->buffer);
                break;
            case RECV:
                send_packet(&(argument->socket), RECV, message_pool->used - last_unread_message);

                for(i = last_unread_message; i < message_pool->size; i++){
                    send_packet(&argument->socket, MESG, strlen(((struct message_t*)(message_pool->data[i]))->buffer));
                    send_message((struct message_t*)(message_pool->data[i]), &argument->socket);
                }
                break;
            case TERM:
                run = 0;
                break;
            default:
                printf("%d %d", packet.command, TERM);
                die("Ambiguous command");
        }
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
