// Client code

#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>

#define PORT 5000 
#define MAXLINE 1024 

//char *jk[100] = {"101","102"};

int main(int argc,char* argv[]) 
{ 
    int sockfd,skfd;
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr,clientaddr; 
    socklen_t l = sizeof(servaddr);
  
    int n, len , opt = 1,ghf; 
     
    // printf("Socket created in C!\n");
    for(int jkc=1;jkc<argc;jkc++){  
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        servaddr.sin_family = AF_INET; 
        servaddr.sin_port = htons(PORT);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
        if (ghf = connect(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) { 
            printf("\n%d Error : Connect Failed \n",ghf); 
        } 
        memset(buffer, 0, sizeof(buffer)); 
        //strcpy(buffer, "101"); 
        // printf("Sending message %s:\n",buffer);
        getsockname(sockfd,(struct sockaddr*)&clientaddr,&l);
        //printf("The port number is: %d\n",htons(clientaddr.sin_port));
        printf("The special code is %d\n",atoi(argv[jkc]));
        //read(STDIN_FILENO,buffer,sizeof(buffer));
        send(sockfd,argv[jkc],sizeof(buffer) ,0);
        printf("Message Sent!\n"); close(sockfd);
        
        skfd = socket(AF_INET,SOCK_STREAM,0);
        setsockopt(skfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        struct sockaddr_in servaddr2;
        
        servaddr2.sin_family = AF_INET; 
        servaddr2.sin_port = htons(clientaddr.sin_port);
        servaddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
        
        //printf("The port number is: %d %d\n",htons(clientaddr.sin_port),htons(servaddr2.sin_port));
        //printf("The address is: %s %s\n", (char*)clientaddr.sin_addr.s_addr,(char*)(servaddr2.sin_addr.s_addr));

        l = sizeof(servaddr2);
        // recvfrom(skfd,(char*)buffer,strlen(buffer),MSG_WAITALL,(struct sockaddr*)&servaddr2,&l);
    
        bind(skfd, (struct sockaddr*)&servaddr2, sizeof(servaddr2)); 
        listen(skfd, 50);
        int nsfd = accept(skfd,(struct sockaddr*)&servaddr2,&l);
        recv(nsfd,&buffer,sizeof(buffer),0); 
        printf("The message is: %s\n",buffer);
    }
    
    close(skfd);
    return 0;
} 