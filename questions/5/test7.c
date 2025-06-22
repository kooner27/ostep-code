#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int rc = fork();

    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        // Child process
        close(STDOUT_FILENO);  // Close standard output (fd 1)
        printf("This will not appear on the screen\n");
        exit(0);
    } else {
        // Parent process
        printf("Parent is still running (PID %d)\n", getpid());
        write() wait(NULL);  // wait for child
    }

    return 0;
}
