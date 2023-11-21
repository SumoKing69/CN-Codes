#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<unistd.h>
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

# define BUF_LEN 1024
# define ADDRESS1  "mysocket1"
# define ADDRESS2  "mysocket2"

typedef struct message{
    long mesg_type;
    char msg_text[BUF_LEN];
} message;

message msg_msg;

int rsfd,msgid;
int bef_count = 1;
int aft_count = 0;

struct reminder{
    int chance;
    int recv_message;
};

int usd_socket1(){
	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

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

int usd_socket2(){
	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

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


// Thread function
void put_in_queue(void *arg){
	char buf[BUF_LEN];
	struct sockaddr_in cli_addr;
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if((bind(rsfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0)) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    // printf("I am in thread\n");
    socklen_t cli_len = sizeof(cli_addr);
    while(1){
	    recvfrom(rsfd,buf,BUF_LEN,0,(struct sockaddr*)&cli_addr,&cli_len);
        struct iphdr* ip = (struct iphdr*) buf;
        char* cpy = malloc(BUF_LEN);
        strcpy(cpy,(buf+(ip->ihl)*4));
        strcpy(msg_msg.msg_text,cpy);
        printf("%s\n",msg_msg.msg_text);
        msg_msg.mesg_type = 1;
        msgsnd(msgid,&msg_msg,sizeof(msg_msg.msg_text),0);
    }
    
    pthread_exit(0);
}

// struct iphdr *ip;
// ip=(struct iphdr*)buf;
// printf("%s\n",(buf+(ip->ihl)*4));
int main(){
	// Message Queue
    key_t ky = ftok("/home/sumoking69/Desktop/CN",65);
    msgid = msgget(ky , 0666 | IPC_CREAT);

    int sp1_usd = usd_socket1();
    int sp2_usd = usd_socket2();

    struct sockaddr_un ucli_addr1,ucli_addr2;
  	int ucli_len1;
    ucli_len1 = sizeof(ucli_addr1);
    int ucli_len2;
    ucli_len2 = sizeof(ucli_addr2);

    rsfd=socket(AF_INET,SOCK_RAW,IPPROTO_TCP);

	int nusfd1,nusfd2;
	nusfd1 = accept(sp1_usd, (struct sockaddr *)&ucli_addr1, &ucli_len1);
    nusfd2 = accept(sp2_usd, (struct sockaddr *)&ucli_addr2, &ucli_len2);
    

    pthread_t mesg_service;
 	if(pthread_create(&mesg_service,NULL,(void*)&put_in_queue,NULL)!=0)
	    perror("\npthread_create ");
    
    
    // Shared memory for chances!
    key_t k = ftok("shm100", 100);
	int id = shmget(k, sizeof(struct reminder), 0666 | IPC_CREAT);  // Declaring shared memory

	if (id == -1) {
		perror("shmget"); // Check for errors
		exit(1);
	}

    // Attaching the struct to shared memory
    struct reminder *sp_rem;

	sp_rem = (struct reminder*)shmat(id, NULL, 0);	// Attaching the structure in the shared memory
    // sp_rem->chance = 0;

    while(1){
        printf("Sending message to SP1\n");
        sp_rem->chance = 0;
        send_fd(nusfd1,rsfd); 
        while(sp_rem->chance != 100){
            ;
        }
        printf("Sending message to SP2\n");
        sp_rem->chance = 1;
        send_fd(nusfd2,rsfd);
        while(sp_rem->chance != 200){
            ;
        }
    }
}