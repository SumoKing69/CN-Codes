#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_PROCESSES 4

int main(void) {

    printf("Hello\n");
    int i;
    struct pollfd fds[NUM_PROCESSES];
    FILE *fptrs[NUM_PROCESSES];
    char buf[512];

    // // Open pipes to P2, P3, P4, and P5
    fptrs[0] = popen("./P2", "r");
    fptrs[1] = popen("./P3", "r");
    fptrs[2] = popen("./P4", "r");
    fptrs[3] = popen("./P5", "r");

    // // Set up poll structures
    for (i = 0; i < NUM_PROCESSES; i++) {
        //printf("%d\n", fileno(fptrs[i]));
        fds[i].fd = fileno(fptrs[i]);
        fds[i].events=0;
        fds[i].events |= POLLIN;
        fds[i].revents=0;
    }

    // // Wait for data from any of P2, P3, P4, or P5
    int poll_result = poll(fds, NUM_PROCESSES, -1);

    int check = 0;
    // // Check which process has data and feed it to P6
    for (i = 0; i < NUM_PROCESSES; i++) {
        
        if (poll_result == -1) {
            perror("poll");
            return 1;
        }
        if (fds[i].revents & POLLIN) {
            while (fgets(buf, sizeof(buf), fptrs[i])) {
                FILE *p6 = popen("./P6", "w");
                fputs(buf, p6);
                pclose(p6);
            }
            check = 1;
        }
    }

    // // If no data is available from any process, feed data from P1 to P6
    if (i == NUM_PROCESSES && check == 1) {
        FILE *p1 = popen("./P1", "r");
        printf("Hello for P1!\n");
        if (p1) {
            while (fgets(buf, sizeof(buf), p1)) {
                FILE *p6 = popen("./P6", "w");
                fputs(buf, p6);
                pclose(p6);
                //system("./P6");
                char c;
                fgets(c, 1, stdin);
            }
            pclose(p1);
        }
    }

    // // Close pipes to P2, P3, P4, and P5
    for (i = 0; i < NUM_PROCESSES; i++) {
        pclose(fptrs[i]);
    }

    return 0;
}

