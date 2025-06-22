#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    int rc = fork();
    if (rc < 0) {
        perror("fork");
        exit(1);
    } else if (rc == 0) {
        // Child
        close(pipefd[0]);  // Close read end
        printf("hello\n");
        // Signal parent
        write(pipefd[1], "x", 1);
        close(pipefd[1]);
    } else {
        // Parent
        char buf;
        close(pipefd[1]);  // Close write end
        // Block until child writes to pipe
        read(pipefd[0], &buf, 1);
        printf("goodbye\n");
        close(pipefd[0]);
    }

    return 0;
}
