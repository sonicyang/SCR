#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listener.h"
#include "client_handler.h"
#include "setting.h"
#include "pool.h"
#include "misc.h"

struct client_list_t{
    struct client_t** clients;
    int size;
};

static struct client_t* wait_for_client(int* listener_sock, struct client_t* client){
    client->length = sizeof(client->address);
    client->socket = accept(*listener_sock, (struct sockaddr*)&(client->address), &(client->length));
    client->activate = 1;

    if(client->socket < 0){
        die("Failed to accept connection");
    }

    start_client_handler(client);

    return client;
}

int start_listener(struct setting_t* setting, pthread_t* listener_id){
    if(pthread_create(listener_id, NULL, (void*)listener, (void*)setting)){
        die("Failed on creating Listener thread");
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
        if(pool->used_mark[i] == 1){
            pthread_cancel(clients[i]->thread_id);
            pthread_join(clients[i]->thread_id, NULL);
        }
    }

    delete_pool(pool);
}

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;
    int connected_user = 0;
    struct pool_t* clients;
    struct client_t* client;
    int i;

    pthread_cleanup_push(listener_free_socket, &listener_sock);
    pthread_cleanup_push(listener_free_clients, &clients);

    clients = create_pool(sizeof(struct client_t));

    printf("LISTENER   |  Initialzing...\n");
    /*clients_list.clients = (struct client_t**)malloc(setting->max_user * sizeof(struct client_t*));*/
    /*memset(clients_list.clients, 0, setting->max_user * sizeof(struct client_t*));*/
    /*clients_list.size = setting->max_user;*/

    printf("LISTENER   |  Opening Socket...\n");
    listener_sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("LISTENER   |  Socket Opened\n");

    if(listener_sock < 0)
        die("Failed on opening listener socket");

    load_setting(setting, &address);

    if(bind(listener_sock, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) < 0)
        die("Server Unable to bind Address");

    printf("LISTENER   |  Listening...\n");
    if(listen(listener_sock, setting->max_user))
        die("Cannot listen on the socket");

    while(1){
        client = pool_allocate(clients);
        wait_for_client(&listener_sock, client);
        connected_user++;

        /*XXX: Pass Information between threads are more propreiate*/
        for(i = 0; i < clients->size; i++){
            if(((struct client_t*)(clients->data[i]))->activate == -1){
                pthread_join(client->thread_id, NULL);
                pool_free(clients, clients->data[i]);
                client->activate = 0;
            }
            client--;
        }
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return;
}

