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

#define ADDRESS "ESSsocket"

int idx = 0, left_idx, right_idx=0, flag1=0, flag2=1;

// Structure for shared memory
struct msg{
	int i;
	char cli_num[100][4];
	int pids[100];
};

struct msg *m;

int main(){
	key_t k = ftok("shm100", 100); 
	int id = shmget(k, sizeof(struct msg), 0666 | IPC_CREAT);  // Declaring shared memory

    shmctl(id,IPC_RMID,NULL);
}