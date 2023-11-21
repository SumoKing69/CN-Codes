#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
	char str[100];
	scanf("%s",str);
    strncat(str," Froms2",strlen(" Froms2")+1);
	printf("in p2: %s\n",str);
}