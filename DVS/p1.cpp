#include "networks.h"
#include <bits/stdc++.h>
using namespace std;
int stationMaster = -1;
struct sockaddr_un uaddr;

void handleError(int res, char *msg)
{
    if (res < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

// ############### UNIX SOCKETS ###############
int createUSocket()
{
    int res = socket(AF_UNIX, SOCK_STREAM, 0);
    handleError(res, "createUSock");
    return res;
}

void configUSocket(char *path)
{
    uaddr.sun_family = AF_UNIX;
    strcpy(uaddr.sun_path, path);
}

void bindUSocket(int usfd)
{
    if (bind(usfd, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0)
        perror("Could not bind");
    else
        printf("Bind successful\n");
}

void listenUSocket(int usfd)
{
    if (listen(usfd, 10) < 0)
        perror("Could not listen");
    else
        printf("Listen successful\n");
}

int getPID(char *fileName)
{
    const char *command = "pidof ./";
    char *finalCommand;
    finalCommand = (char *)malloc(strlen(fileName) + 1 + strlen(command));
    strcpy(finalCommand, command);
    strcat(finalCommand, fileName);
    int pid = fileno(popen(finalCommand, "r"));
    return pid;
}

// ############### SIGNAL PASSING ###############
struct msg_buffer
{
    long msg_type;
    int ppid;
} message;
int signalPid = -1;

void get_pid(int sig, siginfo_t *info, void *context)
{
    signalPid = info->si_pid;
}

int recieveSignal()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &get_pid;
    if (sigaction(SIGUSR1, &sa, NULL) < 0)
        cout << "\nError in sigaction ";
    pause();
    printf("\nPID of the signal sender = %d\n", signalPid);
    return signalPid;
}

void sendSignal(int ppid)
{
    if (kill(ppid, SIGUSR1) < 0)
        cout << "\nError in sending kill" << endl;
    else
        cout << "\nSIGUSR1 sent to process " << ppid << endl;
}

// SHARED MEMORY
struct shm_buffer
{
    int platformStatus[3] = {0, 0, 0};
};
shm_buffer *mem;
key_t shm_key;
int shm_id;

void initializeSHM()
{
    shm_key = ftok("/", 65);
    shm_id = shmget(shm_key, sizeof(shm_buffer), 0666 | IPC_CREAT);
    mem = (shm_buffer *)shmat(shm_id, NULL, 0);
}

void readFromTrain(int nsfd)
{
    while (1)
    {
        char buf[1024];
        int sz = recv(nsfd, buf, 1024, 0);
        if (sz == 0)
            break;
        cout << sz << " "
             << "Train: " << buf << "\n";
    }
}

void informSM(int usfd)
{
    mem->platformStatus[0] = 0;
    char msg[1024] = "Train has left the station!";
    send(usfd, msg, sizeof(msg), 0);
}

int main()
{
    cout << "PID: " << getpid() << endl;
    // use progfile to get this working
    stationMaster = getPID("sm");

    initializeSHM();

    // Create a socket connection
    int usfd = createUSocket();
    configUSocket("p1.socket");

    // Connect to server
    if (connect(usfd, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0)
        perror("Could not connect");
    else
        cout << "Connected successfully!\n";

    // listen for requests
    while (1)
    {
        char buf[1024];
        int sz = recv(usfd, buf, 1024, 0);
        cout << sz << "I am here\n";
        // 2 => train incoming
        if (sz && mem->platformStatus[0] == 0)
        {
            printf("Train coming in\n");
            if (mem->platformStatus[0] == 0)
            {
                cout << "I am free! So I will accept the train\n";
                int nsfd = getFD(usfd);
                mem->platformStatus[0] = 1;
                cout << "Connection recieved: " << nsfd << endl;
                readFromTrain(nsfd);
                informSM(usfd);
            }
            else
            {
                // not free
                cout << "Train is weird, but here\n";
            }
        }
    }

    return 0;
}