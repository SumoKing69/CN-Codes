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

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";

void func(int usfd){
    for(int i=0;i<5;i++){
        char buff[1024];
        recv(usfd,buff,sizeof(buff),0);
        printf("From Super Server: %s\n",buff);
        memset(buff,0,sizeof(buff));
    }
}

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
    func(nufd);
}

int main(int argc, char **argv) {
    int sfds;
    // int visit[4] = {0};

    // for(int i = 0; i < 4; i++) {
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
    // for(int i = 0; i < 4; i++) {
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        my_addr.sin_port = htons(8000);
        socklen_t addrlen = sizeof(my_addr);
        if(bind(sfds, (struct sockaddr*)&my_addr, addrlen) < 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        if(listen(sfds, 100) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    // }

    while(1) {
        int nsfd = accept(sfds, (struct sockaddr*)&my_addr, &addrlen);

        int c = fork();
        if(c == 0) {
            // dup2(sfds[i],service_fd[i]);          
            execv("/home/sumoking69/Desktop/CN/Internet_Super_Server_Reports/S1",NULL);

        }
        pass_fd(nsfd);
    }
}