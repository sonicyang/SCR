#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#include "listener.h"
#include "client_handler.h"
#include "setting.h"
#include "pool.h"
#include "misc.h"
#include "message.h"

struct client_list_t{
    struct client_t** clients;
    int size;
};

struct list_t* message_list;

static int wait_for_client(int* listener_sock, struct client_t* client){
    client->length = sizeof(client->address);
    client->socket = accept(*listener_sock, (struct sockaddr*)&(client->address), &(client->length));
    client->activate = 1;

    if(client->socket < 0){
        print_err("Failed to accept connection");
        return 1;
    }

    return start_client_handler(client);
}

int start_listener(struct setting_t* setting, pthread_t* listener_id){
    if(pthread_create(listener_id, NULL, (void*)listener, (void*)setting)){
        print_err("Failed on creating Listener thread");
        return 1;
    }
    return 0;
}

static void listener_free_socket(void* argument){
    int* sock = (int*)argument;
    close(*sock);
}

static void listener_free_clients(void* argument){
    struct pool_t* pool = (struct pool_t*)*(struct pool_t**)argument;
    struct client_t** clients = (struct client_t**)pool->data;
    int i;

    for(i = 0; i < pool->size; i++){
        if(pool->used_mark[i] == 1 && clients[i]->activate == 1){
            pthread_cancel(clients[i]->reciver_thread_id);
            pthread_join(clients[i]->reciver_thread_id, NULL);
            shutdown(clients[i]->socket, SHUT_WR);
        }
    }

    delete_pool(pool);
}

static void listener_free_sems(void* argument){
    struct list_t* list = (struct list_t*)*(struct list_t**)argument;
    struct list_element_t* ptr = list->head;

    while(ptr != NULL){
        sem_destroy(ptr->data);
        ptr = ptr->next;
    }

    delete_list(list);
}

static void listener_free_message_pool(void* argument){
    struct list_t* list = (struct list_t*)*(struct list_t**)argument;
    while(list->head != NULL)
        list_free(list, list_pop(list));

    delete_list(message_list);
}

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;
    int connected_user = 0;
    struct pool_t* clients;
    struct list_t* client_sems;
    struct client_t* client;
    int i;

    pthread_cleanup_push(listener_free_message_pool, &message_list);
    pthread_cleanup_push(listener_free_socket, &listener_sock);
    pthread_cleanup_push(listener_free_clients, &clients);
    pthread_cleanup_push(listener_free_sems, &client_sems);

    clients = create_pool(sizeof(struct client_t));
    client_sems = create_list(sizeof(sem_t*));
    message_list = create_list(sizeof(struct message_t));

    printf("LISTENER   |  Initialzing...\n");

    printf("LISTENER   |  Opening Socket...\n");
    listener_sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("LISTENER   |  Socket Opened\n");

    if(listener_sock < 0){
        print_err("Failed on opening listener socket");
        close(listener_sock);
        return;
    }

    load_setting(setting, &address);

    if(bind(listener_sock, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) < 0){
        print_err("Server Unable to bind Address");
        close(listener_sock);
        return;
    }

    printf("LISTENER   |  Listening...\n");
    if(listen(listener_sock, setting->max_user)){
        print_err("Cannot listen on the socket");
        close(listener_sock);
        return;
    }

    while(1){
        client = pool_allocate(clients);
        client->activate = 0;
        client->sem_list = client_sems;
        if(wait_for_client(&listener_sock, client)){
            connected_user++;
        }

        /*XXX: Pass Information between threads are more propreiate*/
        for(i = 0; i < clients->size; i++){
            if(((struct client_t*)(clients->data[i]))->activate == -1){
                pthread_join(client->reciver_thread_id, NULL);
                shutdown(((struct client_t*)(clients->data[i]))->socket, SHUT_WR);
                pool_free(clients, clients->data[i]);
                client->activate = 0;
            }
            client--;
        }
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return;
}

