#include <stdio.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

int main(){
    FILE* ftpr;
    char toread[1000];
    int i,ch;
    ftpr = fopen("/home/ak2k02/207110/CN/GDB_V1/P.cpp","r");
    for(i=0;i<(sizeof(toread)-1) && ((ch = fgetc(ftpr)) != EOF) && !feof(ftpr); i++){
        toread[i] = ch;
    }
    toread[i] = '\0';
    fclose(ftpr);

    int fd = open("/home/ak2k02/207110/CN/GDB_V1/famous_fifo",O_WRONLY);
    write(fd,&toread,strlen(toread)+1);
    close(fd);
    memset(toread,0,sizeof(toread));
    fd = open("famous_fifo",O_RDONLY);
    read(fd,toread,sizeof(toread));
    close(fd);
    printf("%s\n" , toread);
}