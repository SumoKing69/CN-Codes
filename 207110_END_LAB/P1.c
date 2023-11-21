#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<netinet/tcp.h>
#include<sys/ipc.h>
#include<sys/msg.h>

# define buff1 "Received from Server" 

int main(){
	int rsfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);
    struct sockaddr_in serv_addr;

    // int tcpfd = socket(AF_INET,SOCK_STREAM,0);

    // serv_addr.sin_family = AF_INET;
	// serv_addr.sin_port = htons(8080);
	// serv_addr.sin_addr.s_addr = INADDR_ANY;
    // socklen_t serv_len = sizeof(serv_addr);
    
	// int opt=1;
	// setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	// if(bind(tcpfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))==-1)
	// perror("\n bind : ");
	// else printf("\n bind successful ");

	// listen(tcpfd,10);
    // int new_nsfd = accept(tcpfd , (struct sockaddr*)&serv_addr , &serv_len);

	int optval=1;
	setsockopt(rsfd, IPPROTO_TCP, SO_BROADCAST, &optval, sizeof(int));
	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr("127.0.0.1");

	char buff[]="Sending information to Organiser!";	
	socklen_t client_len=sizeof(client);
    int count = 0;
    while(1){
        sendto(rsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,client_len); count++;
    }
    // Create a TCP socket
}
