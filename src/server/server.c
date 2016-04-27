#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "misc.h"

#define OPTION "f:"

struct setting_t{
    char address[16];
    int32_t port;
};

int32_t parse_setting(const char* setting_fn, struct setting_t* setting){
    char pname[32];
    char pvalue[32];

    FILE *fil;
    fil = fopen(setting_fn, "r");

    if(!fil){
        return 1;
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
        }
    }

    fclose(fil);

    return 0;
}

int start_linstiner(struct setting_t* setting){

}

int main(int argc, char *argv[])
{
    int cmd_opt = 0;
    char setting_fn[64] = "\0";

    struct setting_t setting;

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

    if(parse_setting(setting_fn, &setting)){
        die("Error parsing setting");
    }

    exit(0);

     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"die, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        die("die opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              die("die on binding");
     listen(sockfd,5);
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
