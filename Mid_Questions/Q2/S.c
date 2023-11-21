// Server Code

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

int service_fd[] = {10,20,30,40};
char *service_path[] = {"S1","S2","S3","S4"};
int ports[] = {8010,8020,8030,8040};

int max(int a,int b){
    if(a > b){
        return a;
    }
    else{
        return b;
    }
}

int main(int argc, char **argv) {
    int sfds[4];
    int visit[4] = {0};

    for(int i = 0; i < 4; i++) {
        sfds[i] = socket(AF_INET, SOCK_STREAM, 0);
        if(sfds[i] == 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }
        // int opt = 1;
        // if (setsockopt(sfds[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }
    }

    struct sockaddr_in my_addr[4];
    for(int i = 0; i < 4; i++) {
        my_addr[i].sin_family = AF_INET;
        my_addr[i].sin_addr.s_addr = INADDR_ANY;
        my_addr[i].sin_port = htons(ports[i]);
        socklen_t addrlen = sizeof(my_addr[i]);
        if(bind(sfds[i], (struct sockaddr*)&my_addr[i], addrlen) < 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        if(listen(sfds[i], 10) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    fd_set rfds;

    int maxi = -1;
    for(int i = 0; i < 4; i++)
        maxi = max(maxi, sfds[i]);

    while(1) {
        FD_ZERO(&rfds);
        for(int i = 0; i < 4; i++) {
            FD_SET(sfds[i], &rfds);
            maxi = max(maxi, sfds[i]);
        }
        int act = select(maxi + 1, &rfds, NULL, NULL, NULL);
            for(int i = 0; i < 4; i++) {
                if(FD_ISSET(sfds[i], &rfds)) {
                    int c = fork();
                    if(c == 0) {
                        dup2(sfds[i],service_fd[i]);  
                        execv(service_path[i],NULL);
                    }
                }
            }
    }
}
