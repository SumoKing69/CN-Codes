#include <stdio.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define WRITE 1
#define READ 0
#define buffsize 1000

int main(){
    int pd;
    printf("In child now!");
    pid_t in = 20, ot = 21;
    char read_buff[50], write_buff[50];
    size_t sz = buffsize;

    int read_end = dup(WRITE);
    int write_end = dup(READ);

    while(1){
        read(read_end,&read_buff,buffsize);
        if(strcmp(read_buff,"\n") == 0){
            exit(0);
        }
        dup2(in,READ); // Read has ot
        printf("Process 2: Receiving message: %s\n",read_buff);
        dup2(ot,WRITE); // write has in
        memset(write_buff,0,sizeof(write_buff));
        printf("Process 2: Sending message:");
        scanf("%s",write_buff);
        write(write_end,write_buff,buffsize);
    }
    close(read_end);
    close(write_end);
}