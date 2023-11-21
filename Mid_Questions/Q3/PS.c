// Proxy Server 

#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5000 
#define MAXLINE 1024
char *superserver[100] = {"8000","8001","8002","8003","8004"} ;
struct sockaddr_in client;
int sss;
pthread_mutex_t lock[5];

char buffer[MAXLINE];

void* func(void* n){
    int sfd; 
    struct sockaddr_in sear; 
    printf("Waiting for the lock!\n");
    pthread_mutex_lock(&lock[sss]);
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    sear.sin_family = AF_INET; 
    sear.sin_port = htons(atoi(superserver[sss])); 
    sear.sin_addr.s_addr = inet_addr("127.0.0.1");  
    // printf("Socket created in C!\n");
    connect(sfd, (struct sockaddr*)&sear,sizeof(sear));
    char buff[1024] , port[10];
    
    sprintf(port,"-%d",(client.sin_port));
    strcat(buff,inet_ntoa(client.sin_addr));
    strcat(buff,port);
    printf("Sending message to SS: %s\n",buff);
    send(sfd,buff,sizeof(buff),0);
    pthread_mutex_unlock(&lock[sss]);
    memset(buff,0,sizeof(buff));
    printf("Message sent and lock is released!\n");
}

int main() 
{ 
    int sockfd,opt=1; 
    struct sockaddr_in servaddr; 
    int n, len; 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("socket creation failed"); 
        exit(0); 
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    listen(sockfd, 50);
    socklen_t l = sizeof(client);
    //while(1){
        // printf("Socket created in PS!\n");
    while(1){
        int nsfd = accept(sockfd,(struct sockaddr*)&servaddr,&l);
        recv(nsfd,&buffer,sizeof(buffer),0);
        printf("Message from Proxy Server: %s \n",buffer);
        getpeername(nsfd,(struct sockaddr*)&client,&l);
        for(int i=0;i<5;i++){
            pthread_mutex_init(&lock[i],NULL);
        }
        pthread_t pid[5];
        for(int i=0;i<5;i++){
            char* pk; 
            sprintf(pk,"%d",101+i);
            if(strcmp(pk,buffer) == 0){
                sss = i; 
                pthread_create(&pid[i],NULL,func,NULL);
                pthread_join(pid[i],NULL);
            }
        }
        for(int i=0;i<5;i++){
            pthread_mutex_destroy(&lock[i]);
        }
    //}
    }
    close(sockfd);
    return 0;
} 