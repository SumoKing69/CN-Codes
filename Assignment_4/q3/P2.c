// q3 p2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>

char *S12 = "S12";
char *S23 = "S23";

int main()
{
    sem_t *sem_id12 = sem_open(S12, O_CREAT, 0666, 0);
    sem_t *sem_id23 = sem_open(S23, O_CREAT, 0666, 0);

    while (1)
    {
        printf("I am P2. I am waiting for Semaphore S12\n");
        sem_wait(sem_id12);
        printf("I got semaphore S12 signalling from P1\n");
        printf("Enter any character to sem-signal(S23): ");
        char ch;
        scanf("%c", &ch);
        printf("I am signalling semaphore signal of S23.\n");
        sem_post(sem_id23);
    }
    sem_unlink(S12);
    sem_unlink(S23);
}