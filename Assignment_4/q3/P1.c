// q3 p1
#include <stdio.h>          
#include <stdlib.h>         
#include <unistd.h>
#include <sys/types.h>     
#include <sys/wait.h>             
#include <semaphore.h>      
#include <fcntl.h>

char *S12 = "S12";
char *S41 = "S41";

int main(){
    sem_t *sem_id12 = sem_open(S12, O_CREAT, 0666, 0);
    sem_t *sem_id41 = sem_open(S41, O_CREAT, 0666, 0);

    while(1){
        printf("I am P1. Enter any character to sem-signal(S12): ");
        char ch; scanf("%c",&ch);
        printf("I am signalling signal of S12.\n");
        sem_post(sem_id12);
        printf("I am waiting for semaphore S41\n");
        sem_wait(sem_id41);
        printf("I got semaphore signalling from P4\n");
    }
    sem_unlink(S12);
    sem_unlink(S41);
}