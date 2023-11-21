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

int flag[3];
int nsfd;
#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";

typedef struct message{
    long mesg_type;
    char buff[10];
} message;

int max(int a,int b){
    if(a > b){
        return a;
    }
    else{
        return b;
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
    
    char notify[1000];
    recv(nufd,notify,sizeof(notify),0);
    printf("%s\n",notify);
    char *token = strtok(notify,"-");
    token = strtok(NULL,"-");
    printf("%s\n",token);
    printf("The Train has left platform %d\n",atoi(token));
    flag[atoi(token)-1] = 0;
    unlink(path);
}

int main(int argc,char *argv[]){
    // Message Queue
    message msg_msg;
    key_t ky = ftok("/home/Desktop/CN/Unix_Domain_socket/Sender_Receiver",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);
    
    int fd[3] , opt = 1;
    for(int i=0;i<3;i++){
        fd[i] = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(fd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        flag[i] = 0;
    }
    // Train
    struct sockaddr_in my_addr[3];
    socklen_t addrlen = sizeof(my_addr[0]);
    for(int i = 0; i < 3; i++) {
        my_addr[i].sin_family = AF_INET;
        my_addr[i].sin_addr.s_addr = INADDR_ANY;
        my_addr[i].sin_port = htons(atoi(argv[i+1]));
        addrlen = sizeof(my_addr[i]);
        if(bind(fd[i], (struct sockaddr*)&my_addr[i], addrlen) < 0) {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        if(listen(fd[i], 10) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    fd_set rfds;
    int maxi = -1;
    for(int i = 0; i < 4; i++)
        maxi = max(maxi, fd[i]);

    while(1) {
        FD_ZERO(&rfds);
        for(int i = 0; i < 3; i++) {
            FD_SET(fd[i], &rfds);
            maxi = max(maxi, fd[i]);
        }
        int act = select(maxi + 1, &rfds, NULL, NULL, NULL);
            for(int i = 0; i < 3; i++) {
                if(FD_ISSET(fd[i], &rfds) && flag[i] == 0) {
                    flag[i] = 1;
                    nsfd = accept(fd[i], (struct sockaddr*)&my_addr[i],&addrlen);
                    printf("%d\n",nsfd);
                    // Announcing that a T has arrived!
                    for(int j=0;j<3;j++){
                        if(i == j){
                            printf("From SM train going station: %d\n",j+1);
                            msg_msg.mesg_type = j+1;
                            sprintf(msg_msg.buff,"%d",j+10);
                            msgsnd(msgid,&msg_msg,sizeof(msg_msg.buff),0);
                        }
                        else{
                            printf("From SM train not going station: %d\n",j+1);
                            msg_msg.mesg_type = j+1;
                            sprintf(msg_msg.buff,"%d",j+1);
                            msgsnd(msgid,&msg_msg,sizeof(msg_msg.buff),0);
                        }
                    }
                    // printf("Over till announcing!\n");
                    pass_fd(nsfd);
                }
                //msgctl(msgid, IPC_RMID, NULL);
            }
    }

}

