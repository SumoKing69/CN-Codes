#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CONTROLLGEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cm = NULL;
char *path = "./fsock";

void rec_fd(int fd,int req_fd){
    struct iovec ghf[2];
    char ptr[2] = {0,0};
    int nsfd;

    struct msghdr m;
    ghf[0].iov_base = ptr;
    ghf[0].iov_len = 2;
    m.msg_iov = ghf;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

    cm = malloc(CONTROLLGEN);
    cm->cmsg_level = SOL_SOCKET;
    cm->cmsg_type = SCM_RIGHTS;
    cm->cmsg_len = CONTROLLGEN;
    
    m.msg_control = cm;
    m.msg_controllen = CONTROLLGEN;
    
    *(int*)CMSG_DATA(cm) = req_fd;

    sendmsg(fd,&m,0);
    printf("fd sent from server!\n");
}

int create_socket(){
    int fd;
    fd = socket(AF_UNIX,SOCK_STREAM,0);
    unlink(path);

    struct sockaddr_un server;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path,path);
    //int len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    bind(fd,(struct sockaddr*)&server,sizeof(server));
    listen(fd,50);
    printf("Socket created!\n");
    return fd;
}

int main(int argc,char*argv[]){
    int fd,len;
    struct sockaddr_un client;
    len = sizeof(client);
    fd = create_socket();
    for(int i=1;i<argc;i++){
        int nsfd = accept(fd,(struct sockaddr*)&client,&len);
        printf("Client Accepted!\n");
        char buff[100];
        int req_fd = open("test1.txt",O_RDONLY);
        read(req_fd,buff,20);
        printf("Printing from Server 1: %s\n",buff);
        rec_fd(fd,req_fd);
        memset(buff,0,sizeof(buff));
        recv()
        // read(nsfd,buff,20);
        // printf("%s\n",buff);
        printf("Printing from Server 2: %s\n",buff);
        unlink(path);
    }
}
