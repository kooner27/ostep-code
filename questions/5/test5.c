#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
wait returns the pid of the terminated child
you can pass in a pointer or reference to status to store the exit code
you can decode it with macros like WIFEXITED(status)

if the child clals wait it will return error
*/

int main() {
    int rc = fork();

    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        // Child process
        printf("Child (PID %d) is running\n", getpid());
        sleep(1);
        printf("Child done\n");
    } else {
        // Parent process
        printf("Parent (PID %d) is waiting for child (PID %d)...\n", getpid(), rc);
        int status;
        pid_t child_pid = wait(&status);  // Wait for child to finish
        printf("Parent got child PID %d from wait()\n", child_pid);

        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
