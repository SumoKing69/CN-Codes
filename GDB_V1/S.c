#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>

int main(){
    // Reading the P.cpp file
    char read_arr[1000];
    mkfifo("/home/ak2k02/207110/CN/GDB_V1/famous_fifo", 0666);
    int fd = open("famous_fifo",O_RDONLY);
    read(fd,read_arr,sizeof(read_arr));
    close(fd);
    
    // creating another .cpp file
    FILE* ftpr;
    ftpr = fopen("/home/ak2k02/207110/CN/GDB_V1/P1.cpp","w");
    if(strlen(read_arr) > 0){
        fputs(read_arr,ftpr);
    }
    fclose(ftpr);

    // Compiling the P1.cpp file
    sleep(1);
    system("g++ P1.cpp -o P1.exe");

    // Forking S
    int ffpd = fork();
    if(ffpd > 0){
        // Parent Process
        wait();
        FILE* actual_otp , *expected_otp;
        char act_arr[100] , exp_arr[100];
        int ch;

        // Reading the outputs
        actual_otp = fopen("/home/ak2k02/207110/CN/GDB_V1/Pout.txt","r"); 
        expected_otp = fopen("/home/ak2k02/207110/CN/GDB_V1/OT.txt","r");
        int flag = 1;
        while(fgets(act_arr, 100, actual_otp) != NULL && fgets(exp_arr, 100, expected_otp) != NULL){
            int len1 = strlen(act_arr), len2 = strlen(exp_arr);
            for(int i = 0; i<len2; i++)
                if(exp_arr[i] != act_arr[i]){
                    flag = 0;
                    break;
                }
            if(!flag) break;
        }  
        fclose(actual_otp); fclose(expected_otp);

        // Opening Fifo to write the result
        fd = open("/home/ak2k02/207110/CN/GDB_V1/famous_fifo",O_WRONLY);
        if(flag){
            write(fd,"Test Case Passed",strlen("Test Case Passed")+1);
        }else{
            write(fd,"Test Case Failed",strlen("Test Case Failed")+1);
        }
        close(fd);
    }
    else{
        // Child Process
        int fp1 = open("/home/ak2k02/207110/CN/GDB_V1/IT.txt",O_RDONLY);
        int fp2 =open("/home/ak2k02/207110/CN/GDB_V1/Pout.txt",O_WRONLY);
        dup2(fp1 , STDIN_FILENO); dup2(fp2 , STDOUT_FILENO);
        close(fp1); close(fp2);
        execlp("/home/ak2k02/207110/CN/GDB_V1/P1.exe", NULL);
    }

}