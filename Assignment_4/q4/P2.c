// q4 p2
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h> 
#include <sys/shm.h>
#include <pthread.h>
#include <fcntl.h>

char *S1 = "S1";
char *S2 = "S2";
char *x,*y;

int main(){
    // Creating semaphores
    sem_t *sem_id1 = sem_open(S1, O_CREAT, 0666, 0);
    sem_t *sem_id2 = sem_open(S2, O_CREAT, 0666, 0);

    // Creating shared memory for x
    key_t shmkey1 = ftok("/home/ak2k02/207110/CN/Assignment_4/q4", 65);
    int shmid1 = shmget(shmkey1, sizeof(int), 0644 | IPC_CREAT);
    x = (char*) shmat(shmid1,NULL,0);

    // Creating shared memory for y
    key_t shmkey2 = ftok("/home/ak2k02/207110/CN/Assignment_4/q4", 66);
    int shmid2 = shmget(shmkey2, sizeof(int), 0644 | IPC_CREAT);
    y = (char*)shmat(shmid2, NULL, 0);
    char st[5];
    int a = 0;
    while (1)
    {
        printf("\nI am waiting for S1.\n");
        sem_wait(sem_id1);  // Waiting
        x = (char*) shmat(shmid1, NULL, 0); // Reading x
        a = atoi(x); 
        printf("The value of x is: %d\n",a);
        a++;    // Increamenting by 1
        sprintf(st,"%d",a);
        strcpy(y,st);   // Writing to y
        printf("Enter any char to signal S2: ");
        char ch;sleep(1);
        scanf("%c", &ch);   // Input
        sleep(1);
        sem_post(sem_id2);  // Signalling S2
    }
    // Destroying the semaphores
    sem_unlink(S1);
    sem_unlink(S2);
    // Detach and destory
    shmdt(x);
    shmctl(shmid1, IPC_RMID, NULL);
    shmdt(y);
    shmctl(shmid1, IPC_RMID, NULL);
}