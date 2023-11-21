#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cm = NULL;
char *path = "./fsock";

int rec_fd(int fd){
    struct iovec ghf[2];
    char ptr[2];
    int nsfd;

    struct msghdr m;
    ghf[0].iov_base = ptr;
    ghf[0].iov_len = sizeof(ptr);
    m.msg_iov = ghf;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

    cm = malloc(CONTROLLEN);
    m.msg_control = cm;
    m.msg_controllen = CONTROLLEN;

    recvmsg(fd,&m,0);
    printf("fd received from client!\n");
    nsfd = *(int*)CMSG_DATA(cm);
    return nsfd;
}

int create_socket(char *name){
    int fd;
    fd = socket(AF_UNIX,SOCK_STREAM,0);
    unlink(name);

    struct sockaddr_un server,client;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,name);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    bind(fd,(struct sockaddr*)&server,len);
printf("Heelo!\n");
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path,path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    connect(fd,(struct sockaddr*)&client,len);
    printf("Connected!\n");
    return fd;
}

int main(){
    int fd,nsfd,len;
    char* name = "./csock";
    fd = create_socket(name);
    for(int i=0;i<2;i++){
        nsfd = rec_fd(fd);
        char buff[100];
        printf("Received nsfd %d",nsfd);
        read(nsfd,buff,20);
        printf("Printing from Client 1: %s\n",buff);
        memset(buff,0,sizeof(buff));
        send(fd,buff,sizeof(buff),0);
    }
    unlink(name);
}