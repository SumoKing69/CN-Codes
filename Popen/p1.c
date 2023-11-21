#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
	FILE *fp=popen("./p2","w");
	dup2(fileno(fp),1);
	printf("Fromp1\n");
	return 0;
}