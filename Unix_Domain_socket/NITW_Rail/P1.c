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

# define ID 1 
int flag ;

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";

typedef struct message{
    long mesg_type;
    char buff[10];
} message;

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
    // printf("Received fd!\n");
    return newfd;
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
    strcpy(su.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(connect(fd, (struct sockaddr*)&su, len) < 0) 
    {
        perror("connect");
        exit(1);
    }

    return fd;
}

void pass_fd(){
    int ufd, rfd;
    char *name = "./csock";
    // printf("Came here now 1!\n");
    ufd = cli_conn(name);
    if((rfd = recv_fd(ufd)) < 0) 
    {
        perror("recv_fd");
        exit(1);
    }
    char buff[100];
    read(rfd,buff,sizeof(buff));
    printf("Message sent by train is: %s\n",buff);
    send(ufd,"Train reached platform-1",strlen("Train reached platform-1")+1,0);
    printf("Done here!\n");
}

int main(){

    message msg_msg;
    key_t ky = ftok("/home/Desktop/CN/Unix_Domain_socket/Sender_Receiver",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);

    while(1){
        flag = 0;
        msgrcv(msgid,&msg_msg,sizeof(msg_msg.buff),ID,0);
        printf("From P: %s\n",msg_msg.buff);
        if(strcmp("1",msg_msg.buff) == 0){
            printf("Train is going to some platform!\n");
        }
        else{
            printf("Train is coming to platform 1!\n");
            flag = 1;
        }
        // printf("Over till receiving message!\n");
        if(flag){
            pass_fd();
        }
    }
    msgctl(msgid, IPC_RMID, NULL);
}
