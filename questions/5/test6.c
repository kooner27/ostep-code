#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t rc = fork();

    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        // Child process
        printf("Child (PID %d) is running\n", getpid());
        sleep(1);
        printf("Child exiting\n");
        exit(42);  // Custom exit code
    } else {
        // Parent process
        printf("Parent (PID %d) waiting for child (PID %d)...\n", getpid(), rc);
        int status;
        pid_t waited_pid = waitpid(rc, &status, 0);  // Wait specifically for `rc`
        printf("waitpid() returned: %d\n", waited_pid);

        if (WIFEXITED(status)) {
            printf("Child exited normally with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child did not exit normally.\n");
        }
    }

    return 0;
}
