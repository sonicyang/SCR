#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <getopt.h>
#include <stdint.h>
#include <ctype.h>
#include <pthread.h>
#include<signal.h>

#include "misc.h"
#include "setting.h"

#define OPTION "f:"

struct client_t{
    struct sockaddr_in address;
    socklen_t length;
    int socket;
    pthread_t thread_id;
};

struct listener_t{
    struct sockaddr_in address;
    int socket;
    pthread_t thread_id;
};

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

struct client_t* wait_for_client(int* listener_sock){
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

void listener_clean_up(void* argument){
    int* sock = (int*)argument;
    close(*sock);
}

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;
    int connected_user = 0;
    struct client_t** clients;
    int i;

    pthread_cleanup_push(listener_clean_up, &listener_sock);

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
    return;
}

int start_listener(struct setting_t* setting, pthread_t* listener_id){
    if(pthread_create(listener_id, NULL, (void*)listener, (void*)setting)){
        die("Failed on creating Listener thread");
    }
    return 0;
}

struct listener_t listener_instance;

void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
        pthread_cancel(listener_instance.thread_id);
}

int main(int argc, char *argv[]){
    int cmd_opt = 0;
    char setting_fn[64] = "\0";
    struct setting_t setting;

    if (signal(SIGINT, signal_handler) == SIG_ERR)
        die("Cannot handle SIGINT");

    while(1){
        cmd_opt = getopt(argc, argv, OPTION);
        if(cmd_opt == -1)
            break;
        switch (cmd_opt) {
            case 'f':
                strcpy(setting_fn, optarg);
                break;
            case '?':
                fprintf(stderr, "Illegal option:-%c\n", isprint(optopt)?optopt:'#');
                break;
            default:
                fprintf(stderr, "Not supported option exist\n");
                break;
        }
    }

    if(!strlen(setting_fn)){
        die("Must provide a setting file, -fsetting");
    }

    printf("Parsing Setting File....");
    parse_setting(setting_fn, &setting);
    printf("Done\n");

    printf("Creating listener_instance thread...");
    start_listener(&setting, &(listener_instance.thread_id));
    printf("Done\n");

    pthread_join(listener_instance.thread_id, NULL);

    exit(0);

}
