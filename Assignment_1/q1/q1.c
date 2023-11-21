#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define buffsize 1000
#define READ 0
#define WRITE 1

int main()
{
    int parent_to_child[2];
    int child_to_parent[2];
    int pd;

    pipe(parent_to_child);
    pipe(child_to_parent);

    char *write_buffer, *read_buffer;
    size_t sz = buffsize;
    write_buffer = (char *)malloc(sz);
    read_buffer = (char *)malloc(sz);

    pd = fork();
    if (pd > 0)
    {
        // Parent Process
        close(parent_to_child[READ]);
        close(child_to_parent[WRITE]);
        while (1)
        {

            printf("Sending from parent: ");
            getline(&write_buffer, &sz, stdin); // Reading from keyboard
            write(parent_to_child[WRITE], write_buffer, buffsize);

            read(child_to_parent[READ], read_buffer, buffsize);
            if (strcmp(read_buffer, "\n") == 0)
            {
                exit(0);
            }
            printf("Receiving from child: %s\n", read_buffer);
        }
    }
    else
    {
        close(parent_to_child[WRITE]);
        close(child_to_parent[READ]);
        while (1)
        {
            read(parent_to_child[READ], write_buffer, buffsize);
            if (strcmp(write_buffer, "\n") == 0)
            {
                exit(0);
            }
            printf("Receiving from parent: %s\n", write_buffer);

            printf("Sending from child: ");
            getline(&read_buffer, &sz, stdin); // Reading from keyboard
            write(child_to_parent[WRITE], read_buffer, buffsize);
        }
    }
}