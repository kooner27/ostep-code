#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // main process
    int x = 100;

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed");
        exit(1);
    } else if (rc == 0) {
        // child
        x = 1;
        printf("child x: %d\n", x);

    } else {
        // parent
        x = 2;
        int wc = wait(NULL);
        printf("parent x: %d\n", x);
    }
    return 0;
}