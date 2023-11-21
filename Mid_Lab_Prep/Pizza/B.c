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
#include<signal.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";

int send_fd(int fd, int fd_to_send) {
    struct iovec iov[1];
    char ptr[2] = {0, 0};
    int newfd;

    struct msghdr m;

    iov[0].iov_base = ptr;
    iov[0].iov_len = 2;
    m.msg_iov = iov;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

    cmptr = malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    m.msg_control = cmptr;
    m.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = fd_to_send;

    if(sendmsg(fd, &m, 0) < 0) {
        perror("sendmsg");
        return -1;
    }
    // printf("Passing fd done!\n");
    return 0;
}

int serv_listen() {
    int fd, len;
    struct sockaddr_un u;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(path);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(bind(fd, (struct sockaddr*)&u, len)) 
    {
        perror("bind");
        exit(1);
    }

    if(listen(fd, 10) < 0) 
    {
        perror("listen");
        exit(1);
    }

    return fd;
}

void pass_fd(int fd){
    struct sockaddr_un cli_addr;
    int len, ufd, nufd;
    char buff[1000];
    len = sizeof(cli_addr);
    ufd = serv_listen();
    if((nufd = accept(ufd, (struct sockaddr*)&cli_addr, &len)) < 0) 
    {
        perror("accept");
        exit(1);
    }

    if(send_fd(nufd, fd) < 0) 
    {
        perror("send_fd");
        exit(1);
    }
    unlink(path);
}

int main(){
    int fd , opt = 1;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in my_addr;
    socklen_t addrlen = sizeof(my_addr);
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        my_addr.sin_port = htons(8000);
        if(bind(fd, (struct sockaddr*)&my_addr, addrlen) < 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        if(listen(fd, 1000) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    char buff[1024];
    int nsfd = accept(fd, (struct sockaddr*)&my_addr,&addrlen);
    while(1){
        printf("Hello!\n");    
        
        printf("Received Customer!\n");
        // Communication between B and C
        recv(nsfd,buff,sizeof(buff),0);
        printf("The combo offer choice number chosen by customer is: %s\n",buff); 
        // memset(buff,0,sizeof(buff));
        recv(nsfd,buff,sizeof(buff),0);
        printf("Money received!\n");
        pass_fd(nsfd);
        memset(buff,0,sizeof(buff));
    }
}