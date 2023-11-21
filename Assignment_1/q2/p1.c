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

int main()
{
    int pd;
    pid_t in = 20, ot = 21;
    char read_buff[100], write_buff[100];
    size_t sz = buffsize;

    int fd1[2] , fd2[2];
    pipe(fd1);
    pipe(fd2);

    dup2(WRITE,in); // in has 0
    dup2(READ,ot);  // ot has 1

    dup2(fd1[0],WRITE); // Write has READ_END
    dup2(fd2[1],READ);  // read has WRITE_END

    pd = fork();
    if(pd > 0){
        // Parent Process
        
        dup2(in,WRITE); // Write has 0
        dup2(ot,READ);  // Read has 1

        while(1){
            close(fd1[0]);
            close(fd2[1]);
            memset(write_buff,0,sizeof(write_buff));
            printf("Process 1: Sending message: ");
            fgets(write_buff,sz,stdin);
            write(fd1[1],write_buff,buffsize); 
            sleep(1);
            memset(read_buff,0,sizeof(read_buff));
            printf("Gello!");
            read(fd2[0],read_buff,buffsize);
            if(strcmp(read_buff,"\n") == 0){
                exit(0);
            }
            printf("Process 1: Receiving message: %s\n",read_buff);
        }
        close(fd1[1]);
        close(fd2[0]);
    }
    else{
        char *args[] = {"/home/ak2k02/207110/CN/Assignment_1/q2/P2.c",NULL};
	    execv(args[0],args);
    }
}
