#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Optional envp for execle, execvpe
char *envp[] = {"MYVAR=hello", NULL};

int main() {
    int rc = fork();
    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        printf("Child (PID %d) running exec...\n", getpid());

        // You can uncomment one at a time to test:

        // 1. execl - list of args, full path
        // execl("/bin/ls", "ls", "-l", NULL);

        // 2. execle - like execl, but with environment
        // execle("/bin/ls", "ls", "-l", NULL, envp);

        // 3. execlp - like execl, but uses PATH
        // execlp("ls", "ls", "-l", NULL);

        // 4. execv - vector of args, full path
        // char *args[] = { "ls", "-l", NULL };
        // execv("/bin/ls", args);

        // 5. execvp - vector of args, use PATH
        // char *args[] = { "ls", "-l", NULL };
        // execvp("ls", args);

        // 6. execvpe - vector + env + PATH (GNU-only)
        // char *args[] = { "ls", "-l", NULL };
        // execvpe("ls", args, envp);

        // If exec fails
        perror("exec failed");
        exit(1);
    } else {
        wait(NULL);  // parent waits for child
        printf("Parent done.\n");
    }

    return 0;
}
