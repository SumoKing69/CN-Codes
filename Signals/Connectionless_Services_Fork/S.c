#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

# define PORT 8000

int main(int argc , char* argv[]){
    struct sockaddr_in sckaddr;
    int nsfd , sfd , opt = 1; 
    char read_buff[50];
    int addrlen = sizeof(sckaddr); 
    //memset(sckaddr,'\0',sizeof(sckaddr));

    fd_set select_fd; 
    sfd = socket(AF_INET,SOCK_STREAM,0);

    sckaddr.sin_family = AF_INET;
    sckaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sckaddr.sin_port = htons(PORT);

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    bind(sfd,(struct sockaddr*)&sckaddr,sizeof(sckaddr));
    listen(sfd,10);
    while(1){
        printf("Waiting for a request!\n");
        nsfd = accept(sfd,(struct sockaddr*)&sckaddr,(socklen_t*)&addrlen);
        if(nsfd < 0){
            exit(0);
        }
        int fk = fork();
        if(fk == 0){
            while(1){
                memset(read_buff,'\0',sizeof(read_buff));
                recv(nsfd,read_buff,sizeof(read_buff),0);
                printf("Received from client: %s",read_buff);
                strcpy(read_buff,"Message received by the server successfully!");
                send(nsfd,read_buff,sizeof(read_buff),0);
                exit(0);
            }
        }
    }
}