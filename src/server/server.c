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

#include "misc.h"
#include "setting.h"

#define OPTION "f:"

struct client_argument_t{
    struct sockaddr_in address;
    socklen_t length;
    int socket;
    pthread_t thread_id;
};

void client_handler(struct client_argument_t* argument){
     int sockfd, newsockfd, portno;
     char buffer[256];
     int n;
     if (newsockfd < 0)
          die("die on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) die("die reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) die("die writing to socket");
     close(newsockfd);
     close(sockfd);
     return;
}

int start_client_handler(struct client_argument_t* arg){
    if(pthread_create(&(arg->thread_id), NULL, (void*)client_handler, (void*)arg)){
        die("Failed on creating Client Handler thread");
    }
    return 0;
}

struct client_argument_t* wait_for_client(int* listener_sock){
    struct client_argument_t* arg;

    arg = (struct client_argument_t*)malloc(sizeof(struct client_argument_t));
    arg->length = sizeof(arg->address);
    arg->socket = accept(*listener_sock, (struct sockaddr*)&(arg->address), &(arg->length));

    start_client_handler(arg);

    return arg;
}

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;
    int connected_user = 0;
    struct client_argument_t** clients;

    printf("LISTENER   |  Initialzing...\n");
    clients = (struct client_argument_t**)malloc(setting->max_user * sizeof(struct client_argument_t*));

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
        clients[connected_user++] = wait_for_client(&listener_sock);
    }

    close(listener_sock);
    return;
}

int start_listener(struct setting_t setting, pthread_t* listener_id){
    if(pthread_create(listener_id, NULL, (void*)listener, (void*)&setting)){
        die("Failed on creating Listener thread");
    }
    return 0;
}

int main(int argc, char *argv[]){
    int cmd_opt = 0;
    char setting_fn[64] = "\0";
    struct setting_t setting;

    pthread_t  listener_id;


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

    printf("Creating Listener thread...");
    start_listener(setting, &listener_id);
    printf("Done\n");

    pthread_join(listener_id, NULL);

    exit(0);

}
