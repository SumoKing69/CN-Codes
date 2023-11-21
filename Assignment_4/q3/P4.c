// q3 p4
#include <stdio.h>          
#include <stdlib.h>         
#include <unistd.h>
#include <sys/types.h>     
#include <sys/wait.h>    
#include <errno.h>          
#include <semaphore.h>      
#include <fcntl.h>

char *S34 = "S34";
char *S41 = "S41";

int main(){
    sem_t *sem_id34 = sem_open(S34, O_CREAT, 0666, 0);
    sem_t *sem_id41 = sem_open(S41, O_CREAT, 0666, 0);

    while (1)
    {
        printf("I am P4. I am waiting for Semaphore S34\n");
        sem_wait(sem_id34);
        printf("I got semaphore S34 signalling from P3\n");
        printf("Enter any character to sem-signal(S41): ");
        char ch;
        scanf("%c", &ch);
        printf("I am signalling semaphore signal of S41.\n");
        sem_post(sem_id41);
    }
    sem_unlink(S34);
    sem_unlink(S41);
}