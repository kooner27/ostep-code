#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
    struct timeval start, end;
    int iterations = 1000000;

    gettimeofday(&start, NULL);
    for (int i = 0; i < iterations; i++) {
        read(0, NULL, 0);  // 0-byte syscall
    }
    gettimeofday(&end, NULL);

    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Total time: %ld us for %d calls\n", elapsed, iterations);
    printf("Avg time per syscall: %.2f ns\n", (elapsed * 1000.0) / iterations);

    return 0;
}

// so 3.3ghz computer. giga is 10^9
// we like 1 clock cycle every 3.3 nano seconds

// we determined x nano seconds for 1000000 syscalls
// or about 230 nano seconds / syscall

// so a syscall is about 80 clock cycles