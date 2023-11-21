// q3 p3
#include <stdio.h>          
#include <stdlib.h>         
#include <unistd.h>
#include <sys/types.h>     
#include <sys/wait.h>    
#include <errno.h>          
#include <semaphore.h>      
#include <fcntl.h>

char *S23 = "S23";
char *S34 = "S34";

int main(){
    sem_t *sem_id23 = sem_open(S23, O_CREAT, 0666, 0);
    sem_t *sem_id34 = sem_open(S34, O_CREAT, 0666, 0);

    while (1)
    {
        printf("I am P3. I am waiting for Semaphore S23\n");
        sem_wait(sem_id23);
        printf("I got semaphore S23 signalling from P2\n");
        printf("Enter any character to sem-signal(S34): ");
        char ch;
        scanf("%c", &ch);
        printf("I am signalling semaphore signal of S34.\n");
        sem_post(sem_id34);
    }
    sem_unlink(S23);
    sem_unlink(S34);
}