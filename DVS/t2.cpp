#include "networks.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT 8001
struct sockaddr_in serv_addr;

int createSocket()
{
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    return sock;
}

void configAddress()
{
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
}

void connectSocket(int sock)
{
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }
    printf("Connection established successfully!\n");
}

int main(int argc, char const *argv[])
{
    cout << "PID: " << getpid() << endl;
    int sock, valread;
    char buff[1024] = {0};

    // Create a socket
    sock = createSocket();

    // Configure address
    configAddress();

    // Connect socket to server
    connectSocket(sock);

    char rcv[1024] = "Schedule: 10-11-12-1-2-3";
    cout << "\nSchedule: " << rcv << "\n";
    cout
        << send(sock, rcv, strlen(rcv) + 1, 0) << "\n";
    sleep(10);
    cout << "Leaving station now..."
         << "\n";
    cout << send(sock, "Leaving now....", 17, 0) << "\n";
    return 0;
}