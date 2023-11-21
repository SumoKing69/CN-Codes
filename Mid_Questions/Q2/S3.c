#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

void *S3_func(void* n){
    int* nsfd = (int*)n;
    char buff[1024];
    recv(nsfd,buff,sizeof(buff),0);
    printf("%s \n",buff);
    printf("Message received!\n");
    memset(buff,0,sizeof(buff));
}

int main(int argc,char* argv[]){
    // int sfd; 
    // sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(8030);

    socklen_t addrlen = sizeof(my_addr);
    // if(bind(sfd, (struct sockaddr*)&my_addr, addrlen) < 0) {
    //     perror("bind");
    //     exit(EXIT_FAILURE);
    // }
    // if(listen(sfd, 20) < 0) {
    //     perror("listen");
    //     exit(EXIT_FAILURE);
    // }

    char buff[1024];
    pthread_t id;
    
    while(1){
        int nsfd = accept(30, (struct sockaddr*)&my_addr, &addrlen);
        printf("I am in child of S3!\n");
        pthread_create(&id,NULL,S3_func,(void*)nsfd);
        pthread_join(id,NULL);
    }
}