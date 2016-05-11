#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "list.h"
#include "misc.h"
#include "tranciver.h"
#include "tui.h"
#include "packet.h"
#include "message.h"

void connect_server(struct TUI_t* tui, char* input, void* argument){
    struct client_tranciver_t* tmp = (struct client_tranciver_t*)argument;
    struct hostent *server;

    server = gethostbyname(input);
    if (server == NULL) {
        die("No such host");
    }
    memset((char *) &tmp->server_address, 0, sizeof(struct sockaddr_in));
    tmp->server_address.sin_family = AF_INET;
    memcpy(&tmp->server_address.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    //XXX: Hard Coded Port number
    tmp->server_address.sin_port = htons(8787);

    tmp->tui = tui;

    start_tranciver(tmp);

    return;
}

void stop_server(struct TUI_t* tui, void* argument){

}

void input_handler(struct TUI_t* tui, char* input, void* argument){
    struct client_tranciver_t* tmp = (struct client_tranciver_t*) argument;
    struct message_t* message;
    int* command;
    if(tmp->packet_to_send && tmp->message_to_send){
        command = list_allocate(tmp->packet_to_send)->data;
        message = list_allocate(tmp->message_to_send)->data;
        *command = MESG;
        init_message(message, tmp->name, strlen(input));
        strcpy(message->buffer, input);
        sem_post(tmp->sent);
    }
}

void transmitter_clean_up(void* argument){
    struct client_tranciver_t* tmp = (struct client_tranciver_t*) argument;
    delete_list(tmp->message_to_send);
    delete_list(tmp->packet_to_send);
    sem_close(tmp->sent);
    sem_unlink("TRANS_SEMB");
}

void client_transmitter(struct client_tranciver_t* argument){
    struct list_element_t* ptr1;
    struct list_element_t* ptr2;
    struct list_element_t* ptrm;

    argument->message_to_send = create_list(sizeof(struct message_t));
    argument->packet_to_send = create_list(sizeof(struct packet_t));
    sem_unlink("TRANS_SEMB");
    argument->sent = sem_open("TRANS_SEMB", O_CREAT, S_IRWXU, 0);
    pthread_cleanup_push(transmitter_clean_up, argument);

    while(1){
        sem_wait(argument->sent);

        ptr1 = argument->packet_to_send->head;
        while(ptr1 != NULL){
            switch(*(int*)(ptr1->data)){
                case MESG:
                    ptrm = list_pop(argument->message_to_send);
                    send_packet(&argument->socket, MESG, (((struct message_t*)(ptrm->data))->size));
                    send_message(((struct message_t*)(ptrm->data)), &argument->socket);
                    list_free(argument->message_to_send, ptrm);
                    break;
            }
             ptr2 = ptr1;
            list_delete(argument->packet_to_send, ptr2);
            list_free(argument->packet_to_send, ptr2);
             ptr1 = ptr1->next;
        }
    }

    pthread_cleanup_pop(1);
}

void client_clean_up(void* argument){
    struct client_tranciver_t* tmp = (struct client_tranciver_t*) argument;
    pthread_cancel(tmp->transmitter_thread_id);
    pthread_join(tmp->transmitter_thread_id, NULL);
    send_packet(&tmp->socket, TERM, 0);
    close(tmp->socket);
}

void client_reciver(struct client_tranciver_t* argument){
    struct packet_t packet;
    struct message_t message;
    int run = 1;

    //XXX: should print error message on screen and continue, remove die()
    argument->socket = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(argument->socket, (struct sockaddr*)&argument->server_address, sizeof(struct sockaddr_in)) < 0){
        die("ERROR connecting");
    }

    if(pthread_create(&argument->transmitter_thread_id, NULL, (void*)client_transmitter, (void*)argument)){
        die("Failed on creating transmitter thread");
    }

    pthread_cleanup_push(client_clean_up, argument);

    while(run){
        wait_for_packet(&argument->socket, &packet);

        switch(packet.command){
            case MESG:
                init_message(&message, "", packet.parameter);
                recv_message(&message, &argument->socket);

                TUI_write_message(argument->tui, &message);
                break;
            case RECV:
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

void start_tranciver(struct client_tranciver_t* arg){
    if(pthread_create(&(arg->reciver_thread_id), NULL, (void*)client_reciver, (void*)arg)){
        die("Failed on creating Client Reciver thread");
    }

    arg->run = 1;
    return;
}

