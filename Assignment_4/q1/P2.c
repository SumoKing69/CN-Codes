#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/msg.h>

typedef struct message {
    long mesg_type;
    char msg_txt[50];
} message;

pid_t left , right;

void handler2(int sig , siginfo_t *siginfo , void* context){
    printf("I am in Process 2.\n");
    left = siginfo->si_pid;
}

void rhandler2(int sig , siginfo_t *siginfo , void* context){
    printf("[r]I am in Process 2.\n");
}

int main()
{
    // Sigaction
    struct sigaction act;
    memset(&act,'\0',sizeof(act));
    act.sa_sigaction = &handler2;
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGUSR1,&act,NULL);

    // For message queue
    message msg_msg;
    key_t ky = ftok("/home/ak2k02/207110/CN/Assignment_4",65);
    int msgid = msgget(ky , 0666 | IPC_CREAT);
    //msgctl(msgid, IPC_RMID, NULL);

    msg_msg.mesg_type = 2;
    sprintf(msg_msg.msg_txt,"%d",getpid());
    msgsnd(msgid,&msg_msg,sizeof(msg_msg.msg_txt),0); // Sending message to process 1

    pause(); // Wait for signal from process 1

    msgrcv(msgid,&msg_msg,sizeof(msg_msg.msg_txt),3,0);
    right = atoi(msg_msg.msg_txt);
    
    sprintf(msg_msg.msg_txt,"%d",left);
    msg_msg.mesg_type = 2;
    msgsnd(msgid,&msg_msg,sizeof(msg_msg.msg_txt),0);   // Sending message containing pid1
    kill(right,SIGUSR1);
    for(int j=0;j<2;j++){
        pause();
        kill(right,SIGUSR1);
    }

    // Reverse signalling
    memset(&act,'\0',sizeof(act));
    act.sa_sigaction = &rhandler2;
    act.sa_flags=SA_SIGINFO;
    sigaction(SIGUSR2,&act,NULL);

    for(int j=0;j<3;j++){
        pause();
        kill(left,SIGUSR2);
    }
}