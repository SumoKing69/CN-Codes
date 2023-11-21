#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/msg.h>

typedef struct message{
    long mesg_type;
    char msg_text[50];
} message;

int left , right;

void handler1(int sig , siginfo_t *siginfo , void* context){
    printf("I am in Process 1.\n");
    left = siginfo->si_pid;
}

void rhandler1(int sig , siginfo_t *siginfo , void* context){
    printf("[r]I am in Process 1.\n");
}

int main()
{
    // Sigaction
    struct sigaction act;
    memset(&act,'\0',sizeof(act));
    act.sa_sigaction = &handler1;
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGUSR1,&act,NULL);

    // For message queue
    message msg_msg;
    key_t ky = ftok("/home/ak2k02/207110/CN/Assignment_4",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);
    msg_msg.mesg_type = 1;
    //msgctl(msgid, IPC_RMID, NULL);

    msgrcv(msgid,&msg_msg,sizeof(msg_msg.msg_text),2,0); // Receiving message from P2
    int right = atoi(msg_msg.msg_text);
    for(int j=0;j<3;j++){
        kill(right,SIGUSR1);
        pause();
    }

    // Reverse signalling
    memset(&act,'\0',sizeof(act));
    act.sa_sigaction = &rhandler1;
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGUSR2,&act,NULL);

    for(int j=0;j<3;j++){
        kill(left,SIGUSR2);
        pause();
    }
}