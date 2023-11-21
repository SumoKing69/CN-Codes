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
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stddef.h>

#define PORT 6000
#define ADDRESS "ESSsocket"

int main(){
	struct sockaddr_in userv_addr,ucli_addr; 
  	int userv_len,ucli_len;
  	int usfd = socket(AF_INET , SOCK_STREAM , 0);
  	
	userv_addr.sin_addr.s_addr = IN_LOOPBACKNET;
	userv_addr.sin_family = AF_INET;
	userv_addr.sin_port = htons(6000);
	userv_len = sizeof(userv_addr); 
  	
  	struct sockaddr_un userv_addr1;
  	int userv_len1; 
	
	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1) 
		perror("server: bind"); 

	listen(usfd, 100);
 	
 	printf("Waiting for socket accept!\n");
	
	int nusfd = accept(usfd, (struct sockaddr *)&userv_addr, &userv_len); 
	printf("Accepted\n");
	while(1){	
		char buffer[1000] = "ESS Serving client";
		write(nusfd, buffer, sizeof(buffer));
		memset(buffer,0,sizeof(buffer));
		read(nusfd, buffer, sizeof(buffer));
		printf("Client: %s\n", buffer);
		memset(buffer,0,sizeof(buffer));
	}
}
