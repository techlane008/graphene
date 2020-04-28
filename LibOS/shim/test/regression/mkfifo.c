#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_PATH "fifo123"

int main(int argc, char** argv) {
    int fd;
    char buffer[1024];
    size_t bufsize = sizeof(buffer);

    if (mkfifo(FIFO_PATH, S_IRWXU) < 0) {
        perror("mkfifo error");
        return 1;
    }

    int pid = fork();

    if (pid < 0) {
        perror("fork error");
        return 1;
    } else if (pid == 0) {
        /* client */
        fd = open(FIFO_PATH, O_RDONLY, S_IRWXU);
        if (fd < 0) {
            perror("[child] open error");
            return 1;
        }

        if (read(fd, &buffer, bufsize) < 0) {
            perror("[child] read error");
            return 1;
        }
        buffer[bufsize - 1] = '\0';

        if (close(fd) < 0) {
            perror("[child] close error");
            return 1;
        }

        printf("read on FIFO: %s\n", buffer);
    } else {
        /* server */
        fd = open(FIFO_PATH, O_WRONLY, S_IRWXU);
        if (fd < 0) {
            perror("[parent] open error");
            return 1;
        }

        snprintf(buffer, bufsize, "Hello from write end of FIFO!");
        if (write(fd, &buffer, strlen(buffer) + 1) < 0) {
            perror("[parent] write error");
            return 1;
        }

        if (close(fd) < 0) {
            perror("[parent] close error");
            return 1;
        }

        wait(NULL); /* wait for child termination, just for sanity */

        if (unlink(FIFO_PATH) < 0) {
            perror("[parent] unlink error");
            return 1;
        }
    }

    return 0;
}