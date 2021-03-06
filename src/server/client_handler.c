#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>


#include "client_handler.h"
#include "list.h"
#include "packet.h"
#include "pool.h"
#include "message.h"
#include "misc.h"

static void brocast_message_recv(struct list_t* sems){
    struct list_element_t* ptr;

    //A client disconnected may cause NULL pointer
    pthread_mutex_lock(sems->lock);

    ptr = sems->head;
    while(ptr != NULL){
        sem_post(*((sem_t**)(ptr->data)));
        ptr = ptr->next;
    }

    pthread_mutex_unlock(sems->lock);
    return;
}

void transmitter_clean_up(void* argument){
    struct client_t* tmp = (struct client_t*) argument;
    char sem_name[64];

    sprintf(sem_name, "%d", (int)pthread_self());
    sem_close(*((sem_t**)((struct list_element_t*)tmp->sem)->data));
    sem_unlink(sem_name);
    list_delete(tmp->sem_list, tmp->sem);
    list_free(tmp->sem_list, tmp->sem);
}

void client_transmitter(struct client_t* argument){
    struct list_element_t* last_read_message = message_list->head;
    char sem_name[64];

    sprintf(sem_name, "%d", (int)pthread_self());

    argument->sem = list_allocate(argument->sem_list);
    sem_unlink(sem_name);
    *((sem_t**)((struct list_element_t*)argument->sem)->data) = sem_open(sem_name, O_CREAT, S_IRWXU, 1);

    pthread_cleanup_push(transmitter_clean_up, argument);

    while(1){
        sem_wait(*((sem_t**)((struct list_element_t*)argument->sem)->data));

        if(message_list->head != NULL){
            if(last_read_message == NULL){
                last_read_message = message_list->head;
                send_packet(&argument->socket, MESG, (((struct message_t*)(last_read_message->data))->size));
                send_message(((struct message_t*)(last_read_message->data)), &argument->socket);
            }
        }else{
            continue;
        }

        while(last_read_message->next != NULL){
            last_read_message = last_read_message->next;
            send_packet(&argument->socket, MESG, (((struct message_t*)(last_read_message->data))->size));
            send_message(((struct message_t*)(last_read_message->data)), &argument->socket);
        }
    }

    pthread_cleanup_pop(1);
}

void client_clean_up(void* argument){
    struct client_t* tmp = (struct client_t*) argument;
    pthread_cancel(tmp->transmitter_thread_id);
    pthread_join(tmp->transmitter_thread_id, NULL);
    tmp->activate = -1;
}

void client_reciver(struct client_t* argument){
    struct packet_t packet;
    struct list_element_t* message;
    int run = 1;

    if(pthread_create(&argument->transmitter_thread_id, NULL, (void*)client_transmitter, (void*)argument)){
        print_err("Failed on creating Client Transmitter thread");
        return;
    }

    pthread_cleanup_push(client_clean_up, argument);

    while(run){
        wait_for_packet(&argument->socket, &packet);

        switch(packet.command){
            case MESG:
                message = list_allocate(message_list);
                init_message(message->data, "", packet.parameter);
                recv_message(message->data, &argument->socket);
                brocast_message_recv(argument->sem_list);

                printf("Here is the message: %s\n", ((struct message_t*)(message->data))->buffer);
                break;
            case RECV:
                break;
            case TERM:
                run = 0;
                break;
            default:
                printf("%d %d", packet.command, TERM);
                print_err("Ambiguous command");
        }
    }

    pthread_cleanup_pop(1);
    return;
}

int start_client_handler(struct client_t* arg){
    if(pthread_create(&(arg->reciver_thread_id), NULL, (void*)client_reciver, (void*)arg)){
        print_err("Failed on creating Client Reciver thread");
        return 1;
    }
    return 0;
}
