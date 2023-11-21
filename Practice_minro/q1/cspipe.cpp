#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

int main(int argc, char *argv[]){
	cout<<"Enter input to "<<argv[1]<<": \n";
	//fflush(stdout);
	FILE *rfp;
	for(int i=1;i<argc;i++){
		rfp=popen(argv[i],"r");
		dup2(fileno(rfp),0);
	}
	char c[100];
	while(fgets(c, 100, rfp)){
		cout<<c;
    }
	return 0;
}