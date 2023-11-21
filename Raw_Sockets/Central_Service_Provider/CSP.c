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
#include<netinet/in.h>
#include<netdb.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stddef.h>

#define ADDRESS1 "mysocket1"
#define ADDRESS2 "mysocket2"

int send_fd(int socket, int fd_to_send)
 {
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;

    /* at least one vector of one byte must be sent */
    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;

    /* initialize socket message */
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    /* provide space for the ancillary data */
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = available_ancillary_element_buffer_space;

    /* initialize a single ancillary data element for fd passing */
    control_message = CMSG_FIRSTHDR(&socket_message);
    control_message->cmsg_level = SOL_SOCKET;
    control_message->cmsg_type = SCM_RIGHTS;
    control_message->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *) CMSG_DATA(control_message)) = fd_to_send;

    return sendmsg(socket, &socket_message, 0);
 }

int serv_listen1() {
    int  usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket1");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS1);
	unlink(ADDRESS1);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
    return usfd;
}

int serv_listen2() {
	int  usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket2");

  	bzero(&userv_addr,sizeof(userv_addr));

  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS2);
	unlink(ADDRESS2);
	userv_len = sizeof(userv_addr);

	if(bind(usfd, (struct sockaddr *)&userv_addr, userv_len)==-1)
	perror("server: bind");

	listen(usfd, 5);
    return usfd;
}

int main(){
    int usfd1,usfd2,nufd1,nufd2;
    struct sockaddr_un ucli_addr1 , ucli_addr2;
    int ucli_len1 = sizeof(ucli_addr1);
    int ucli_len2 = sizeof(ucli_addr2);

    usfd1 = serv_listen1(); usfd2 = serv_listen2();
	nufd1 = accept(usfd1, (struct sockaddr *)&ucli_addr1, &ucli_len1);
    nufd2 = accept(usfd2, (struct sockaddr *)&ucli_addr2, &ucli_len2);
    printf("Accepted all!\n");
    printf("%d\n",send_fd(nufd1,8094));
    // printf("%d\n",send_fd(nufd2,8096));
}