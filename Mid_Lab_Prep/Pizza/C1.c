#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include <stddef.h>
#include<signal.h>

int main(int argc,char *argv[]){
    int fd , opt = 1;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in my_addr;
    socklen_t addrlen = sizeof(my_addr);
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = INADDR_ANY;
        my_addr.sin_port = htons(8000);
    char buff[1024]; 
    connect(fd, (struct sockaddr*)&my_addr, addrlen);   
    for(int i=1;i<argc;i++){    
        printf("Its connected!\n");
        memset(buff,0,sizeof(buff));
        // fgets(buff,sizeof(buff),stdin);
        send(fd,argv[i],sizeof(argv[i]),0); // Combo offer part
        printf("Message sent!\n");
        sleep(1);
        send(fd,"#",strlen("#")+1,0); // Money part
        recv(fd,buff,sizeof(buff),0);
        printf("%s\n",buff);
        sleep(2);
    }
}