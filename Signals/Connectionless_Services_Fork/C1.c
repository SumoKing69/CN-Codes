#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <sys/select.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

# define PORT 8000

int main(){

    struct sockaddr_in sckaddr;
    int nsfd , sfd; 
    char *read_buff = "This is for multiplication";
    char rbuff[1000];

    sfd = socket(AF_INET,SOCK_STREAM,0);

    sckaddr.sin_family = AF_INET;
    sckaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sckaddr.sin_port = htons(PORT);

    connect(sfd,(struct sockaddr*)&sckaddr,sizeof(sckaddr));
    //strcpy(read_buff,);
    printf("Hello!\n");
    send(sfd,read_buff,strlen(read_buff)+1,0);
    printf("Message sent from C1!\n");
    sleep(3);
    recv(sfd,rbuff,sizeof(rbuff),0);
    printf("Message received from server: %s\n",rbuff);
}