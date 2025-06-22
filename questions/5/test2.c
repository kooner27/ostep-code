#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void noisy_write(int fd, const char *label, int repeat) {
    for (int i = 0; i < repeat; i++) {
        write(fd, label, strlen(label));
        usleep(rand() % 1000);  // Add tiny delay between writes
    }
}

int main() {
    int fd = open("./myfile", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd == -1) {
        perror("error opening file");
        exit(1);
    }

    srand(time(NULL) ^ getpid());

    int rc = fork();
    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        noisy_write(fd, "C", 200);  // write "C" 200 times
        printf("child done\n");
    } else {
        noisy_write(fd, "P", 200);  // write "P" 200 times
        printf("parent done\n");
        wait(NULL);
    }

    close(fd);
    return 0;
}
