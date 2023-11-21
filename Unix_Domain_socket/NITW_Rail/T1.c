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

int main(int argc,char *argv[]){
    int sockfd2,opt=1; 
        struct sockaddr_in servaddr2; 
        socklen_t addrlen = sizeof(servaddr2);
        if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
            printf("socket creation failed"); 
            exit(0); 
        }
        printf("%d\n",sockfd2);
        setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt));
        servaddr2.sin_family = AF_INET; 
        servaddr2.sin_port = htons(atoi(argv[1])); 
        servaddr2.sin_addr.s_addr = inet_addr("127.0.0.1");
        // bind(sockfd2, (struct sockaddr*)&servaddr2,addrlen);
        connect(sockfd2, (struct sockaddr*)&servaddr2,addrlen);
        char buff[100];
        strcpy(buff,"Train sending its compartments");
        // printf("Connected!\n");
        // close(sockfd2);
        send(sockfd2,buff,sizeof(buff),0);
        printf("%s\n",buff);
        printf("JOB DONE!\n");
        close(sockfd2);
}
