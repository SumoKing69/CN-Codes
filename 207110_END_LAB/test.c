#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/msg.h>

# define BUF_LEN 1024

typedef struct message{
    long mesg_type;
    char msg_text[BUF_LEN];
} message;

message msg_msg;
int main(){ 
    message msg_msg;
    key_t ky = ftok("/home/sumoking69/Desktop/CN",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);

    msgctl(msgid, IPC_RMID, NULL);
}