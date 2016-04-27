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
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#include "misc.h"

#define OPTION "f:"

struct setting_t{
    char address[16];
    int32_t port;
    int32_t max_user;
};

int32_t parse_setting(const char* setting_fn, struct setting_t* setting){
    char pname[32];
    char pvalue[32];
    FILE *fil;

    printf("Parsing Setting File....");

    fil = fopen(setting_fn, "r");

    if(!fil){
        die("Error opening setting");
    }

    while(fscanf(fil, "%s %s", pname, pvalue) != EOF){
        if(!strcmp("ADDR", pname)){
            if(!strcmp("*", pvalue)){
                strcpy(setting->address, "\0");
            }else{
                strncpy(setting->address, pvalue, 15);
                setting->address[15] = '\0';
            }
        }else if(!strcmp("PORT", pname)){
           setting->port = atoi(pvalue);
        }else if(!strcmp("MAX", pname)){
           setting->max_user = atoi(pvalue);
        }
    }

    fclose(fil);

    printf("Done\n");

    return 0;
}

void load_setting(struct setting_t* setting, struct sockaddr_in* address){
    memset((char *) address, 0, sizeof(struct sockaddr_in));

    address->sin_family = AF_INET;
    address->sin_port = htons(setting->port);
    if(!strlen(setting->address)){
       address->sin_addr.s_addr = INADDR_ANY;
    }else{
        if(!inet_pton(AF_INET, setting->address, &(address->sin_addr))){
            die("Bad IPv4 format");
        }
    }

    return;
}

void listener(struct setting_t* setting){
    int listener_sock;
    struct sockaddr_in address;

    printf("LISTENER   |  Opening Socket...\n");
    listener_sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("LISTENER   |  Socket Opened\n");

    if(listener_sock < 0)
        die("Failed on opening listener socket");

    load_setting(setting, &address);

    if(bind(listener_sock, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) < 0){
        die("Server Unable to bind Address");
    }

    printf("LISTENER   |  Listening...\n");
    if(listen(listener_sock, setting->max_user)){
        die("Cannot listen on the socket");
    }

    return;
}

int start_listener(struct setting_t setting, pthread_t* listener_id){

    printf("Creating Listener thread...");
    if(pthread_create(listener_id, NULL, (void*)listener, (void*)&setting)){
        die("Failed on creating Listener thread");
    }
    printf("Done\n");

    pthread_join(*listener_id, NULL);
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

    parse_setting(setting_fn, &setting);

    start_listener(setting, &listener_id);

    exit(0);

     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
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
     return 0;
}
