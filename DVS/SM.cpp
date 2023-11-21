#include "networks.h"
#include <bits/stdc++.h>
#include <semaphore.h>
#include <string.h>
#include <thread>
using namespace std;
#define PATH "fd_passing.socket"

struct sockaddr_in addr[3];
int ports[3] = {8000, 8001, 8002};
// trainSockets
int trainSockets[3] = {0, 0, 0};
int platformPorts[3] = {8003, 8004, 8005};
int platformSockets[3] = {-1, -1, -1};
char *socketPaths[3] = {"p1.socket", "p2.socket", "p3.socket"};

struct sockaddr_un uaddr[3];

void watchPlatforms(int);

void handleError(int res, char *msg)
{
    if (res < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

int createSocket()
{
    int res = socket(AF_INET, SOCK_STREAM, 0);
    handleError(res, "craete Socket");
    return res;
}

int createUSocket()
{
    int res = socket(AF_UNIX, SOCK_STREAM, 0);
    handleError(res, "createUSock");
    return res;
}

void configUSocket(int i, char *path)
{
    uaddr[i].sun_family = AF_UNIX;
    strcpy(uaddr[i].sun_path, path);
}

void bindUSocket(int i, int usfd)
{
    if (bind(usfd, (struct sockaddr *)&uaddr[i], sizeof(uaddr[i])) < 0)
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

int acceptUConn(int i, int usfd)
{
    int addrlen = sizeof(uaddr[i]);
    int res = accept(platformSockets[i], (struct sockaddr *)&uaddr[i], (socklen_t *)&addrlen);
    handleError(res, "accept");
    printf("Connection accepted successfully!\n");
    return res;
}

void setSockOptions(int socketnum, int opt)
{
    int res = setsockopt(trainSockets[socketnum], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    handleError(res, "setsockopt");
}

void bindSocket(int socketnum)
{
    addr[socketnum].sin_family = AF_INET;
    addr[socketnum].sin_addr.s_addr = INADDR_ANY;
    addr[socketnum].sin_port = htons(ports[socketnum]);
    int res = bind(trainSockets[socketnum], (struct sockaddr *)&addr[socketnum], sizeof(addr[socketnum]));
    handleError(res, "bindsocket");
}

void listenSockets(int socketnum)
{
    int res = listen(trainSockets[socketnum], 5);
    handleError(res, "listen");
    printf("Socket %d listening for requests....\n", socketnum + 1);
}

int acceptConn(int socketnum)
{
    int addrlen = sizeof(addr[socketnum]);
    int res = accept(trainSockets[socketnum], (struct sockaddr *)&addr[socketnum], (socklen_t *)&addrlen);
    handleError(res, "accept");
    printf("Connection accepted successfully!\n");
    return res;
}

void sendTrainFD(int nsfd, int platformFD)
{
    struct msghdr msg;
    char buffer[2];
    struct iovec iov[1];
    iov[0].iov_base = buffer;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    struct cmsghdr *cmptr;
    cmptr = (struct cmsghdr *)malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = nsfd;
    if (sendmsg(platformFD, &msg, 0) < 0)
        perror("Send: Could not send");
    else
        printf("Send: Success..\n");
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
    for (int i = 0; i < 3; i++)
    {
        mem->platformStatus[i] = 0;
    }
}

bool isPlatformEmpty()
{
    for (int i = 0; i < 3; i++)
    {
        if (mem->platformStatus[i] == 0)
            return true;
    }
    return false;
}

void handleTrain(int nsfd)
{
    // send annoucement of train to all platforms
    char *msg = "0";
    for (int i = 0; i < 3; i++)
    {
        cout << send(platformSockets[i], msg, strlen(msg) + 1, 0) << endl;
        // perror("Send: ");
    }

    // send nsfd to closest vacant platform
    for (int i = 0; i < 3; i++)
    {
        if (mem->platformStatus[i] == 0)
        {
            sendTrainFD(nsfd, platformSockets[i]);
            // sendFD(nsfd, platformSockets[i]);
            cout << "Train sent to platform " << i << endl;
            break;
        }
    }
}

void watchTrains()
{

    while (1)
    {
        fd_set readset;
        FD_ZERO(&readset);
        int max = -1;
        for (int i = 0; i < 3; i++)
        {
            FD_SET(trainSockets[i], &readset);
            if (trainSockets[i] > max)
                max = trainSockets[i];
        }

        struct timeval t;
        t.tv_sec = 3;
        t.tv_usec = 100;
        int rv = select(max + 1, &readset, NULL, NULL, &t);

        if (rv == -1)
            perror("select");
        else
        {
            int i;
            // check for events
            for (i = 0; i < 3; i++)
            {
                if (FD_ISSET(trainSockets[i], &readset))
                {
                    printf("Train arrived at station %d\n", i);
                    // check if platform is empty, if yes then accept
                    if (isPlatformEmpty())
                    {
                        // accept train
                        int nsfd = acceptConn(i);
                        // handle train
                        handleTrain(nsfd);

                        // THE FUNCTION TRACKS PLATFORMS' MESSAGES

                        // thread second(watchPlatforms, i);
                        // second.join();
                    }
                    else
                    {
                        // do what?
                    }
                }
            }
        }
    }
}

void watchPlatforms(int i)
{
    while (1)
    {
        char buf[1024];
        recv(platformSockets[i], buf, 1024, 0);
        cout << "Platform " << i + 1 << ": " << buf << "\n";
    }
}

void initializeSockets()
{
    // create, bind, and listen to train sockets
    for (int i = 0; i < 3; i++)
    {
        trainSockets[i] = createSocket();
        setSockOptions(i, 1);
        bindSocket(i);
        listenSockets(i);
    }

    // create, bind and listen to platform sockets
    for (int i = 0; i < 3; i++)
    {
        platformSockets[i] = createUSocket();
        configUSocket(i, socketPaths[i]);
        bindUSocket(i, platformSockets[i]);
        listenUSocket(platformSockets[i]);
    }
}

int main()
{
    cout << "PID: " << getpid() << endl;

    // INITIALIZE SOCKETS
    initializeSockets();

    // INITIALIZE SHARED MEMORY
    initializeSHM();

    int nsfd1 = acceptUConn(0, platformSockets[0]);
    platformSockets[0] = nsfd1;

    // UNCOMMENT THIS TO WORK WITH 2 PLATFORMS

    // int nsfd2 = acceptUConn(1, platformSockets[1]);
    // platformSockets[1] = nsfd2;

    // watch for incoming trains and platform messages
    thread first(watchTrains);

    first.join();

    return 0;
}