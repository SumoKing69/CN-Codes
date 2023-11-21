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

# define ADDRESS2 "mysocket2"
# define BUFF_LEN 1024
# define buff1 "From client!"

struct reminder{
    int chance;
    int recv_message;
};

typedef struct message{
    long mesg_type;
    char msg_text[1024];
} message;

message msg_msg;

int usd_socket(){
	int  usfd;
	struct sockaddr_un userv_addr,ucli_addr;
  	int userv_len,ucli_len;

	usfd = socket(AF_UNIX , SOCK_STREAM , 0);
	perror("socket");

  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
    strcpy(userv_addr.sun_path, ADDRESS2); 
    userv_len = sizeof(userv_addr);

    if(connect(usfd,(struct sockaddr *)&userv_addr,userv_len)==-1){
	    perror("\n connect ");
    }
	else{printf("\nconnect succesful");}

    return usfd;
}

 int recv_fd(int socket)
 {
  int sent_fd, available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
   return -1;

  if(message_buffer[0] != 'F')
  {
   /* this did not originate from the above function */
   return -1;
  }

  if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
  {
   /* we did not provide enough space for the ancillary element array */
   return -1;
  }

  /* iterate ancillary elements */
   for(control_message = CMSG_FIRSTHDR(&socket_message);
       control_message != NULL;
       control_message = CMSG_NXTHDR(&socket_message, control_message))
  {
   if( (control_message->cmsg_level == SOL_SOCKET) &&
       (control_message->cmsg_type == SCM_RIGHTS) )
   {
    sent_fd = *((int *) CMSG_DATA(control_message));
    return sent_fd;
   }
  }

  return -1;
 }

void port_handler(int tcpfd){
    // write(tcpfd,"Hello",strlen("Hello")+1);
    // read(tcpfd,buff1,sizeof(buff1));
    // printf("%s\n",buff1);
}

int main(){
    int usfd = usd_socket();
    // printf("Connected to O!\n");
    int tcpfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t serv_len = sizeof(serv_addr);
	
    connect(tcpfd, (struct sockaddr*)&serv_addr, serv_len);

    // Message Queue
    key_t ky = ftok("/home/sumoking69/Desktop/CN",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);

    // Shared memory
    key_t k = ftok("shm100", 100);
	int id = shmget(k, sizeof(struct reminder), 0666 | IPC_CREAT);  // Declaring shared memory

	if (id == -1) {
		perror("shmget"); // Check for errors
		exit(1);
	}

    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    // Attaching the struct to shared memory
    struct reminder *sp_rem;
    message msg_msg;
	sp_rem = (struct reminder*)shmat(id, NULL, 0);	// Attaching the structure in the shared memory

    while(1){
        if(sp_rem->chance == 1){
            int nsfd = recv_fd(usfd); printf("Received fd\n");
            for(int i=0;i<5;i++){ 
                // Reading from message queue!
                char buff[BUFF_LEN];
                msgrcv(msgid,&msg_msg,sizeof(msg_msg.msg_text),1,0); int ans = 0;
                if(msg_msg.msg_text[0] <= '9' && msg_msg.msg_text[0] >= '0'){
                    ans = 1;
                }
                if(ans == 1){
                    getpeername(tcpfd,(struct sockaddr*)&cli_addr,&cli_len);
                    int fg = connect(tcpfd,(struct sockaddr*)&cli_addr,sizeof(cli_addr));
                    port_handler(tcpfd);
                }
                // int ans = port_finder(buff);
                if(ans == 0){
                    printf("Received from SP2 %s\n",msg_msg.msg_text);
                }
            }
            sp_rem->chance = 200;
        }
        else if(sp_rem->chance == 0){
            printf("SP2 : I am speaking!\n");
            while(sp_rem->chance != 100){
                
            }
        }
    }
}