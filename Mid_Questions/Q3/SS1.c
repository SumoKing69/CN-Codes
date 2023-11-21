// Special Server Code

#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <pthread.h>

# define PORT 8000
char buffer[1024];

int main() 
{ 
    int sockfd; 
    struct sockaddr_in servaddr; 
    
    int n, len; 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("socket creation failed"); 
        exit(0); 
    }

    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    listen(sockfd, 50);
    socklen_t l = sizeof(servaddr);
    //while(1){
        // printf("Socket created in PS!\n");
    while(1){
        printf("SS1 waiting for accept!\n");
        int nsfd = accept(sockfd,(struct sockaddr*)&servaddr,&l);
        recv(nsfd,buffer,sizeof(buffer),0);
        printf("Message from Proxy Server: %s \n",buffer);
        //customized_func();
        char* token1 = strtok(buffer,"-");
        char* token2 = strtok(NULL,"-");
        // printf("The port number is: %s",token2);
        // printf("The IP is %s \n",token1);

        int sockfd2,opt=1; 
        struct sockaddr_in servaddr2; 
        
        if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
            printf("socket creation failed"); 
            exit(0); 
        }
        setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));
        //printf("The port number is: %d\n",htons(atoi(token2)));
        servaddr2.sin_family = AF_INET; 
        servaddr2.sin_port = htons(atoi(token2)); 
        servaddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
        //printf("The port number connected is: %d\n",htons(servaddr2.sin_port));
        // printf("The IP address of token is %s: ",token1);
        // printf("The IP address of server is %s: ",inet_ntoa(servaddr2.sin_addr));
        // printf("The address is: %s\n", token);

        
        connect(sockfd2, (struct sockaddr*)&servaddr2,sizeof(servaddr2));
        //printf("Hello I am connected!\n");
        char sample[1000] = {"Hello from SS1!"};
        send(sockfd2,sample,sizeof(sample),0);
        printf("Message sent to client!\n");
    }
}