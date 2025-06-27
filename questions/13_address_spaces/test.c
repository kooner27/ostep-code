#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <megabytes> [duration_seconds]\n", argv[0]);
        return 1;
    }
    // get the mb
    char* str = argv[1];
    char* endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    if (!(errno == 0 && *endptr == '\0')) {
        // errno != 0 so something went wrong usually overflow
        printf("Erro: please pass valid number of bytes\n");
        exit(1);
    }
    if ((errno == ERANGE) || val > INT_MAX || val < INT_MIN) {
        printf("out of bounds\n");
        exit(1);
    }
    int mb = (int)val;
    size_t num_bytes = (size_t)mb * 1024 * 1024;
    // got the mb so now get the duration
    long duration_seconds = -1;
    if (argc == 3) {
        errno = 0;
        duration_seconds = strtol(argv[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || duration_seconds <= 0) {
            fprintf(stderr, "Invalid duration\n");
            return 1;
        }
    }

    // we got the duration and mb
    printf("Allocating %d MB\n", mb);
    char* buffer = malloc(num_bytes);
    if (!buffer) {
        perror("malloc");
        return 1;
    }

    printf("touching memory now\n");
    time_t start = time(NULL);
    size_t i = 0;
    while (1) {
        buffer[i] = (char)i;
        i = (i + 1) % num_bytes;
        if (duration_seconds > 0 && (time(NULL) - start) >= duration_seconds) {
            break;
        }
    }

    free(buffer);  // never reached unless duration specified;
    printf("Finished after %ld seconds.\n", duration_seconds);

    return 0;
}