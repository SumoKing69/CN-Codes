#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;

#define PATH "fd_passing.socket"
#define CONTROLLEN CMSG_LEN(sizeof(int))

void sendFD(int fd, int nsfd)
{
    struct msghdr msg;
    char buffer[2];
    struct iovec iov[1];
    iov[0].iov_base = buffer;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    struct cmsghdr *cmptr;
    cmptr = (struct cmsghdr *)malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = fd;
    if (sendmsg(nsfd, &msg, 0) < 0)
        perror("Send: Could not send");
    else
        printf("Send: Success..\n");
}

int getFD(int usfd)
{
    struct msghdr msg;
    char buffer[2];
    struct iovec iov[1];
    iov[0].iov_base = buffer;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    struct cmsghdr *cmptr;
    cmptr = (struct cmsghdr *)malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;

    if (recvmsg(usfd, &msg, 0) < 0)
    {
        perror("Recv: Could not read");
    }
    else
    {
        printf("Recv: Success..\n");
    }
    int nfd;
    nfd = *(int *)CMSG_DATA(cmptr);
    return nfd;
}