#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

void pin_to_cpu(int cpu) {
    cpu_set_t set;                                  // glibc bit mask
    CPU_ZERO(&set);                                 // initialize all to 0
    CPU_SET(cpu, &set);                             // only allow int cpu // only allow int cpu
    sched_setaffinity(0, sizeof(cpu_set_t), &set);  // syscall wrapper bind the current pid 0 to the set
}

int main() {
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    pid_t pid = fork();
    const int iterations = 10000;
    char buf = 'x';

    if (pid == 0) {
        // child
        pin_to_cpu(0);
        for (int i = 0; i < iterations; i++) {
            read(pipe1[0], &buf, 1);   // wait for parent
            write(pipe2[1], &buf, 1);  // signal parent
        }
    } else {
        // parent
        pin_to_cpu(0);
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for (int i = 0; i < iterations; i++) {
            write(pipe1[1], &buf, 1);  // signal child
            read(pipe2[0], &buf, 1);   // wait for child
        }
        gettimeofday(&end, NULL);
        wait(NULL);

        long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
        printf("Total time for %d round-trips: %ld us\n", iterations, elapsed);
        printf("Context switch time: %.2f us\n", elapsed / (2.0 * iterations));
    }
    return 0;
}
