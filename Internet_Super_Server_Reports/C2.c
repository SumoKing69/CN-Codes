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

int main(int argc, char **argv) {
    int sfds;

    //for(int i = 0; i < 3; i++) {
        sfds = socket(AF_INET, SOCK_STREAM, 0);
        if(sfds == 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(sfds, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
    // }

        struct sockaddr_in my_addr;
    // for(int i = 0; i < 3; i++) {
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        my_addr.sin_port = htons(8000);
        socklen_t addrlen = sizeof(my_addr);
        for(int i=0;i<5;i++){
            connect(sfds,(struct sockaddr*)&my_addr,sizeof(my_addr));
            char buff[1024];
            sprintf(buff,"Hello,How you doing? From 8020");
            printf("%s\n",buff);
            send(sfds,buff,strlen(buff)+1,0);
        }
}