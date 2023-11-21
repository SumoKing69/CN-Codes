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
#include <netinet/in.h>
#include <netdb.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stddef.h>
#include<errno.h>
#define ADDRESS "ESSsocket"

int idx = 0, left_idx, right_idx=0, flag1=0, flag2=1;

// Structure for shared memory
struct msg{
	int i;
	char cli_num[100][4];
	int pids[100];
	int connected_sfd;
};

struct msg *m;

// Finding the first and the next right process in the circular queue
void hd_P1(int sig, siginfo_t *info, void *context){
	printf("Signal received from next process!\n");
	int pid = info->si_pid;
	for(int i = 0; i < m->i; i++){
		if(pid == m->pids[i]){
			if(flag1 == 0){
				if(i==m->i-1)  left_idx = i;
				if(i == 1) right_idx = i;
			}
			else{
				if (i == idx + 1) right_idx = i;
			}
		}
	}
}

// Receiving a message from the last process in the circular queue
void hd_P2(int sig, siginfo_t *info, void *context){
	printf("Signal received from previous process!\n");
	flag2 = 1;	
}

// Receiving the fd from the last process 
int recv_fd(int socket){ 

	int sent_fd, available_ancillary_element_buffer_space; 
	struct msghdr socket_message; 
	struct iovec io_vector[1]; 
	struct cmsghdr *control_message = NULL; 
	char message_buffer[1]; 
	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))]; 

	memset(&socket_message, 0, sizeof(struct msghdr)); 
	memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int))); 

	io_vector[0].iov_base = message_buffer; 
	io_vector[0].iov_len = 1; 
	socket_message.msg_iov = io_vector; 
	socket_message.msg_iovlen = 1; 

	socket_message.msg_control = ancillary_element_buffer; 
	socket_message.msg_controllen = CMSG_SPACE(sizeof(int)); 

	if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0) 
		return -1; 
	if(message_buffer[0] != 'F')
		return -1; 
	if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
		return -1; 

	for(control_message = CMSG_FIRSTHDR(&socket_message); 
	
	control_message != NULL; 
	control_message = CMSG_NXTHDR(&socket_message, control_message)){ 
		if( (control_message->cmsg_level == SOL_SOCKET) && (control_message->cmsg_type == SCM_RIGHTS) ){ 
			sent_fd = *((int *) CMSG_DATA(control_message)); 
			return sent_fd; 
		} 

	} 
	return -1;
}


// Sending the fd to the next process in the queue
int send_fd(int socket, int fd_to_send){
	struct msghdr socket_message; 

	struct iovec io_vector[1]; 
	struct cmsghdr *control_message = NULL; 
	
	char message_buffer[1]; 
	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))]; 
	int available_ancillary_element_buffer_space; 
	
	message_buffer[0] = 'F'; 
	io_vector[0].iov_base = message_buffer; 
	io_vector[0].iov_len = 1; 
	
	memset(&socket_message, 0, sizeof(struct msghdr)); 

	socket_message.msg_iov = io_vector; 
	socket_message.msg_iovlen = 1; 

	available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int)); 
	memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space); 
	socket_message.msg_control = ancillary_element_buffer; 
	socket_message.msg_controllen = available_ancillary_element_buffer_space; 

	control_message = CMSG_FIRSTHDR(&socket_message); 
	control_message->cmsg_level = SOL_SOCKET; 
	control_message->cmsg_type = SCM_RIGHTS; 
	control_message->cmsg_len = CMSG_LEN(sizeof(int)); 

	*((int *) CMSG_DATA(control_message)) = fd_to_send; 
	return sendmsg(socket, &socket_message, 0); 
}

int main(){
	int pid = getpid();
	key_t k = ftok("shm100", 100); 
	int id = shmget(k, sizeof(struct msg), 0666 | IPC_CREAT);  // Declaring shared memory
	
	if (id == -1) {
		perror("shmget"); // Check for errors
		exit(1);
	}
	m = (struct msg*) shmat(id, NULL, 0);

	if (m == (void*)-1) {
	    perror("shmat"); // Check for errors
	    exit(1);
	}

	// Creating BSD Connection oriented socket
	int usfd; 
	
	struct sockaddr_in userv_addr; 
  	int userv_len,ucli_len; 
  	
  	usfd = socket(AF_INET, SOCK_STREAM, 0); 
	printf("The socket id is %d\n",usfd);
  	if(usfd==-1) 
  		perror("\nsocket  "); 
	
	// int flags = fcntl(usfd, F_GETFL, 0);
    // fcntl(usfd, F_SETFL, flags | O_NONBLOCK);
  	
	userv_addr.sin_family = AF_INET;
    userv_addr.sin_addr.s_addr = IN_LOOPBACKNET;
	userv_addr.sin_port = htons(6000);
	userv_len = sizeof(userv_addr); 

	printf("Trying to connect to ESS\n");
	
	if(m->i != 0){
		printf("Some other client is connected to ESS. Getting added to the circular queue now!\n");
		flag1 = 1;
	}
	else {
		printf("\nConnection successful to ESS.\n");
	}

	if(!flag1){
		m->i = 0; 						// Index of process 
	}
	else{
		idx = m->i;
	}
	printf("%d\n", m->i);			
	strcpy(m->cli_num[idx], "4");	// Number of client for identification
	m->pids[idx] = pid;				// Getting the pid of the process for signalling
	m->i++;
	
	// printf("%d, %s, %d\n", m->i, m->cli_num[idx], m->pids[idx]);
  	
  	struct sockaddr_un userv_addr1, ucli_addr1; 
  	int userv_len1,ucli_len1; 

  	// Creating a USD socket for interprocess communication!
  	char *address = "socket4";
  	
  	int usfd1 = socket(AF_UNIX, SOCK_STREAM, 0); 
  	if(usfd1==-1) 
  		perror("\nsocket  ");  
	  	
  	userv_addr1.sun_family = AF_UNIX; 
  	
  	strcpy(userv_addr1.sun_path, address); 
  	unlink(address);
  	userv_len1 = sizeof(userv_addr1); 
	
	if(bind(usfd1, (struct sockaddr *)&userv_addr1, userv_len1)==-1)     // Binding the USD socket
		perror("server: bind"); 

	listen(usfd1, 1); 				// Listening 
 	ucli_len1=sizeof(ucli_addr1); 
	
	// Declaring Signal handlers
	struct sigaction s;
	s.sa_flags = SA_SIGINFO;
	s.sa_sigaction = hd_P1;
	sigaction(SIGUSR1, &s, NULL);
	
	s.sa_sigaction = hd_P2;
	sigaction(SIGUSR2, &s, NULL);

	if(flag1){
		left_idx = idx - 1;

		printf("Sending signal %d %d\n", m->pids[left_idx], m->pids[right_idx]);

		kill(m->pids[left_idx], SIGUSR1);
		kill(m->pids[right_idx], SIGUSR1);
	}

	int count = 0;
	while(1){
		if(flag2){
			flag2 = 0;
			if(flag1 == 1){
				int usfd2 = accept(usfd1, (struct sockaddr *)&ucli_addr1, &ucli_len1);
				usfd = recv_fd(usfd2);
				printf("Here\n");
			}
			else{
				connect(usfd,(struct sockaddr *)&userv_addr,userv_len); flag1 = 1;
			}
			
			char buffer[1000];
			read(usfd, buffer, 1000);
			if(strlen(buffer) > 1){
				printf("\nESS %d: %s\n", ++count , buffer);
			}
			
			sleep(5);

			write(usfd, "Thank you from client 4", strlen("Thank you from client 4")); 
			// printf("Message written!\n");

			char cli_addr[30] = "socket";
			strcat(cli_addr, m->cli_num[right_idx]);
			
			sleep(3);
			// Signalling the next process using the shared memory
			// if(right_idx != idx){
				kill(m->pids[right_idx], SIGUSR2);
				sleep(3);
			
				struct sockaddr_un userv_addr2, ucli_addr2; 
				int userv_len2,ucli_len2; 
				
				char address2[30] = "socket";
				strcat(address2, m->cli_num[right_idx]);
				
				// Passing fds within the processes
				int usfd2 = socket(AF_UNIX, SOCK_STREAM, 0); 
				if(usfd==-1) 
					perror("\nsocket  "); 
					
				bzero(&userv_addr2,sizeof(userv_addr2)); 
				userv_addr2.sun_family = AF_UNIX;
				strcpy(userv_addr2.sun_path, address2); 

				userv_len2 = sizeof(userv_addr2); 
				
				// printf("Connecting to next client\n");
				
				if(connect(usfd2,(struct sockaddr *)&userv_addr2,userv_len2)==-1) 
					perror("\n connect "); 
				// else printf("\nconnect succesful\n"); 
				
				send_fd(usfd2, usfd);
				close(usfd2);
				close(usfd);
			// }
		}
	}
	shmctl(id,IPC_RMID,NULL);
	return 0;
}