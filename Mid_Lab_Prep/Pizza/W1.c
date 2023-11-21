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
char *path1 = "./fsock";
char *path2 = "./c1sock";
int usfd;

int recv_fd(int fd) {

    struct iovec iov[1];
    char ptr[2];
    int newfd;

    struct msghdr m;

    iov[0].iov_base = ptr;
    iov[0].iov_len = sizeof(ptr);
    m.msg_iov = iov;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

    cmptr = malloc(CONTROLLEN);
    m.msg_control = cmptr;
    m.msg_controllen = CONTROLLEN;

    if(recvmsg(fd, &m, 0) < 0) 
    {
        perror("recvmsg");
        return -1;
    }

    newfd = *(int *)CMSG_DATA(cmptr);
    return newfd;
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
    // printf("Passing fd done!\n");
    return 0;
}

int cli_conn(char *name) 
{
    int fd, len;
    struct sockaddr_un u, su;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(name);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if(bind(fd, (struct sockaddr*)&u, len)) 
    {
        perror("bind");
        exit(1);
    }

    su.sun_family = AF_UNIX;
    strcpy(su.sun_path, path1);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path1);
    while(connect(fd, (struct sockaddr*)&su, len) != 0);
    return fd;
}

int serv_listen() {
    int fd, len;
    struct sockaddr_un u;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(path2);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, path2);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path2);
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

int pass_fd1(){
    int ufd, rfd;
    char *name = "./csock";
    ufd = cli_conn(name);
    usfd = ufd;
    if((rfd = recv_fd(ufd)) < 0) 
    {
        perror("recv_fd");
        exit(1);
    }
    return rfd;
}

void pass_fd2(int fd){
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
    unlink(path2);
}

int main(){
    while(1){
        int nsfd = pass_fd1();
        printf("Customer arrived from Billing!\n");
        printf("Food items packed and sent to Delivery!\n");
        pass_fd2(nsfd);
    }
}
