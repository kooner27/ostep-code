#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t child1 = fork();
    if (child1 < 0) {
        perror("fork1 failed");
        exit(1);
    }

    if (child1 == 0) {
        // Child 1: writer
        close(pipefd[0]);                // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);                // Close original write end
        execlp("ls", "ls", NULL);        // Replace with ls
        perror("exec ls failed");
        exit(1);
    }

    pid_t child2 = fork();
    if (child2 < 0) {
        perror("fork2 failed");
        exit(1);
    }

    if (child2 == 0) {
        // Child 2: reader
        close(pipefd[1]);               // Close unused write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
        close(pipefd[0]);               // Close original read end
        execlp("wc", "wc", NULL);       // Replace with wc
        perror("exec wc failed");
        exit(1);
    }

    // Parent: close both ends and wait
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}
