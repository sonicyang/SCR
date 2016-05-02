#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listener.h"
#include "client_handler.h"
#include "setting.h"
#include "misc.h"

static struct client_t* wait_for_client(int* listener_sock){
    struct client_t* arg;

    arg = (struct client_t*)malloc(sizeof(struct client_t));
    arg->length = sizeof(arg->address);
    arg->socket = accept(*listener_sock, (struct sockaddr*)&(arg->address), &(arg->length));

    if(arg->socket < 0){
        die("Failed to accept connection");
    }

    start_client_handler(arg);

    return arg;
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

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;
    int connected_user = 0;
    struct client_t** clients;
    int i;

    pthread_cleanup_push(listener_free_socket, &listener_sock);
    pthread_cleanup_push(free, clients);

    printf("LISTENER   |  Initialzing...\n");
    clients = (struct client_t**)malloc(setting->max_user * sizeof(struct client_t*));
    memset(clients, 0, setting->max_user * sizeof(struct client_t*));

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

    while(connected_user < setting->max_user){
        for(i = 0; i < setting->max_user; i++){
            if(clients[i] == 0){
                clients[i] = wait_for_client(&listener_sock);
                break;
            }
        }
    }

    for(i = 0; i < setting->max_user; i++){
        if(clients[i] != 0)
            pthread_cancel(clients[i]->thread_id);
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return;
}

