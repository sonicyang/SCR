#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int sockfd;
short PORT ;//client port
char buffer[256];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void init()
{
    printf("start client...");
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    printf("my socket file descriptor is %d\n",sockfd);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) == -1){
        perror("fail to connect with server");
        printf("Connection fail\n");
        exit(-1);
    }
    printf("Done\n");

}
void* recv_thread(void* p){
    while(1){
        char buf[100] = {};
		//receive message from socket & put in buffer
		//return -1 error 
        if(recv(sockfd,buf,sizeof(buf),0) <= 0){
			return;
        }
        printf("%s\n",buf);
    }
}
void start(char name[])
{
    //create a thread to listen to update other's message
    pthread_t id;
    pthread_create( &id , 0 , recv_thread , 0 );
    char msg[100] = {};
    while(1)
    {
	    
	
	char buf[100] = {};
        scanf("%s",buf);
        sprintf(msg,"%s:%s",name,buf);
	send(sockfd,msg,strlen(msg),0);

    }
}
int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    PORT = atoi(argv[2]);
    init();
    char name[30];
    printf("please enter your name");
    scanf("%s",name);
    start(name);
    return 0;
}
