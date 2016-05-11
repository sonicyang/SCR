#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "packet.h"
#include "tui.h"
#include "list.h"
#include "misc.h"
#include "tranciver.h"


void clear_message(struct TUI_t* tui, char* input, void* argument){
    tui->line = 1;
    clear_win(tui->message_window);

}

int main(int argc, char *argv[]){
    struct TUI_t tui;
    struct client_tranciver_t tranciver;


    TUI_init(&tui);
    TUI_register_command(&tui, "", (command_handler_t)&input_handler, &tranciver);
    TUI_register_command(&tui, "stop", (command_handler_t)&TUI_stop, &tui);
    TUI_register_command(&tui, "clear", &clear_message, &tui);
    TUI_register_command(&tui, "connect", &connect_server, &tranciver);
    TUI_process(&tui);
    TUI_terminate(&tui);

    return 0;
}

/*int main(int argc, char *argv[]){*/
    /*initscr();*/
    /*printw("Hello World !!!");*/
    /*refresh();*/
    /*getch();*/
    /*endwin();*/

    /*exit(0);*/

    /*int sockfd, portno, n;*/
    /*struct sockaddr_in serv_addr;*/
    /*struct hostent *server;*/

    /*char buffer[256];*/
    /*if (argc < 3) {*/
       /*fprintf(stderr,"usage %s hostname port\n", argv[0]);*/
       /*exit(0);*/
    /*}*/
    /*portno = atoi(argv[2]);*/
    /*sockfd = socket(AF_INET, SOCK_STREAM, 0);*/
    /*if (sockfd < 0)*/
        /*error("ERROR opening socket");*/
    /*server = gethostbyname(argv[1]);*/
    /*if (server == NULL) {*/
        /*fprintf(stderr,"ERROR, no such host\n");*/
        /*exit(0);*/
    /*}*/
    /*bzero((char *) &serv_addr, sizeof(serv_addr));*/
    /*serv_addr.sin_family = AF_INET;*/
    /*bcopy((char *)server->h_addr_list[0],*/
         /*(char *)&serv_addr.sin_addr.s_addr,*/
         /*server->h_length);*/
    /*serv_addr.sin_port = htons(portno);*/
    /*if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)*/
        /*error("ERROR connecting");*/
    /*printf("Please enter the message: ");*/
    /*bzero(buffer,256);*/
    /*struct packet_t packet;*/

    /*while(1){*/
        /*scanf("%s",buffer);*/
        /*packet.command = MESG;*/
        /*send_packet(&sockfd,MESG,sizeof(buffer));*/
        /*n = write(sockfd,buffer,sizeof(buffer));*/

        /*wait_for_packet(&sockfd, &packet);*/
        /*n = read(sockfd, buffer, packet.parameter);*/
        /*buffer[n] = '\0';*/
        /*puts(buffer);*/
    /*}*/
    /*packet.command = TERM;*/
    /*n = write(sockfd,&packet,sizeof(struct packet_t));*/

    /*if (n < 0)*/
         /*error("ERROR writing to socket");*/
    /*bzero(buffer,256);*/

    /*n = read(sockfd,buffer,255);*/
    /*if (n < 0)*/
         /*error("ERROR reading from socket");*/
    /*printf("%s\n",buffer);*/

    /*close(sockfd);*/
    /*return 0;*/
/*}*/
