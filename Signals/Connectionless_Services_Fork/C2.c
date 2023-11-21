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
    char read_buff[50];
    //memset(sckaddr,'\0',sizeof(sckaddr));

    fd_set select_fd; 
    sfd = socket(AF_INET,SOCK_STREAM,0);

    sckaddr.sin_family = AF_INET;
    sckaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sckaddr.sin_port = htons(PORT);

    connect(sfd,(struct sockaddr*)&sckaddr,sizeof(sckaddr));

    send(sfd,"This is for capitalization",strlen("This is for capitalization")+1,0);
    printf("Message sent from C2!\n");
    sleep(2);
    recv(sfd,read_buff,sizeof(read_buff),0);
    printf("Message received from server: %s\n",read_buff);
}