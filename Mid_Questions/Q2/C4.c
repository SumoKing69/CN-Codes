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

int main(int argc, char **argv) {
    int sfds;

    //for(int i = 0; i < 3; i++) {
        sfds = socket(AF_INET, SOCK_STREAM, 0);
        if(sfds == 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        // if (setsockopt(sfdd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }
    //}

        struct sockaddr_in my_addr;
    // for(int i = 0; i < 3; i++) {
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        my_addr.sin_port = htons(8040);
        socklen_t addrlen = sizeof(my_addr);
        
        while(1){
            connect(sfds,(struct sockaddr*)&my_addr,sizeof(my_addr));
            char buff[1024];
            sprintf(buff,"Hello,How you doing? From 8040");
            send(sfds,buff,strlen(buff)+1,0);
        }

}