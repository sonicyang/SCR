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
//#include "linkList.h"
#define option "f:"

int listener_sock;
int fd_list[30] = {0};

void listener(struct setting_t* setting){
    struct sockaddr_in address;
    printf("listener   |  opening socket...\n");
    listener_sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("listener   |  socket opened\n");

    if(listener_sock < 0)
        die("failed on opening listener socket");

    load_setting(setting, &address);
   // printf("%d",(setting->max_user));  //10

    if(bind(listener_sock, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) < 0)
        die("server unable to bind address");

    printf("listener   |  listening...\n");
    if(listen(listener_sock, setting->max_user) == -1)
        die("cannot listen on the socket");
}
//send message to all client
void sendMsgToAll(char* msg,int size){
    int i;
    for(i=0;i<size;i++){
        if(fd_list[i]!=0){
            printf("send to %d\n",fd_list[i]);
            send(fd_list[i],msg,strlen(msg),0);  // target id , message ,msg length 
        }
    }
}

struct client_info_t{
	struct setting_t setting;
	int fd;
};
void service(struct client_info_t* client_info)
{
    int fd = client_info->fd;
    while(1){
        char buf[100] = {};
	//wait until receive a msg
	//if client quit recv return -1
	if(recv(fd,buf,sizeof(buf),0) <= 0){
            printf("fd = %d quit\n",fd);
            int i;
            for(i=0;i<client_info->setting.max_user;i++){
                if(fd == fd_list[i]){
		    //release if client exit
                    fd_list[i] = 0;
                    i = 0;
                }
            }
            printf("quit->fd = %d quit\n",fd);
            return ;
        }
        
	sendMsgToAll(buf,client_info->setting.max_user);
    }
}
//if client join chat room ,accept function will get it file descriptor
//we will create a thread "start_listener" for it
//fd_list contain the clients now joined ,0 means empty 

void start_service(struct setting_t setting)
{	
	printf("start service...\n");
	while(1)
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		printf("sockfd = %d",listener_sock);

		int fd  = accept(listener_sock,(struct sockaddr*)&addr,&len);
								//waiting for client join 
							      //return descroptor number if someone join
		if(fd == -1){
		    printf("client connection fail\n");
		    continue;
		}

		int i=0;
		//if new client join , create a listener ,see start_listener for client function 
		for(i=0;i< setting.max_user;i++)
		{	
			if(fd_list[i] == 0)
			{
				printf("pthread = %d\n",fd);
				fd_list[i] = fd;
				//If another client join , create a new thread for it
				struct client_info_t client_info;
				client_info.setting = setting;
				client_info.fd = fd;

				pthread_t p;
				
				pthread_create(&p,0,(void *)service,&client_info);
				break;
			}
		}

	}
}
int main(int argc, char *argv[]){
    int cmd_opt = 0;
    char setting_fn[64] = "\0";
    struct setting_t setting;
    pthread_t  listener_id;
    while(1){
        cmd_opt = getopt(argc, argv, option);
        if(cmd_opt == -1)
            break;
        switch (cmd_opt) {
            case 'f':
                strcpy(setting_fn, optarg);
                break;
            case '?':
                fprintf(stderr, "illegal option:-%c\n", isprint(optopt)?optopt:'#');
                break;
            default:
                fprintf(stderr, "not supported option exist\n");
                break;
        }
    }

    if(!strlen(setting_fn)){
        die("must provide a setting file, -fsetting");
    }

    parse_setting(setting_fn, &setting);
    listener((void *)&setting);
    start_service(setting);
    exit(0);

     return 0;
}
