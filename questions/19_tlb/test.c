#define _GNU_SOURCE  // macros for scheduling, include b4 sched header
#define _POSIX_C_SOURCE >= 199309L

#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
/*
example
a page is 4096 bytes
size of int is 4 bytes.
1024 integers per page
so our jump is 1024;
say we have 16 pages
1024 * 16 = 16384 integers total

say a TLB can hold 128 entries so 128 pages.
if num_pages < 128. and we loop like a million times.
every access after first loop is blazingly fast cuz of TLB.
but if > 129. then we start to get TLB misses
*/

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_pages> <num_trials> \n", argv[0]);
        return 1;
    }
    int num_pages = atoi(argv[1]);
    long num_trials = atoi(argv[2]);
    int jump = 1024;            // integers per page
    long n = num_pages * 1024;  // may overflow 4096 pages * 1024 integers per page
    int* base = aligned_alloc(4096, n * sizeof(int));

    ///
    // int num_pages = atoi(argv[1]);
    // long num_trials = atoi(argv[2]);
    // // int num_pages = 16;
    // // long num_trials = 1000000;
    // size_t page_size = sysconf(_SC_PAGESIZE);
    //
    // size_t jump = page_size / sizeof(int);  // ints per page, 1024
    // size_t n = jump * num_pages;            // elements or int total
    // size_t* base = aligned_alloc(page_size, n * sizeof(int));
    ///

    volatile int* array = base;
    if (!array) {
        fprintf(stderr, "ERROR: allocation failed for %ld pages\n", n);
        return 1;
    }

    // zero out first element in page
    // sometimes the memory is not given until needed
    // this way the pages will have actually been allocated
    for (int i = 0; i < n; i += jump) {
        array[i] = 0;
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    // using the newer posix timer that has ns precision
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // workload
    // access a page everytime
    for (int t = 0; t < num_trials; t++) {
        for (int i = 0; i < n; i += jump) {
            array[i] += 1;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    time_t sec = end.tv_sec - start.tv_sec;
    long nsec = end.tv_nsec - start.tv_nsec;
    // if negative borrow a second
    if (nsec < 0) {
        sec -= 1;
        nsec += 1000000000;
    }

    double elapsed = sec + nsec / 1e9;
    // printf("elapsed: %.9f sec\n", elapsed);

    double total_accesses = (double)num_trials * num_pages;
    double ns_per_access = (elapsed * 1e9) / total_accesses;
    printf("%d\t%ld\t%.2f\n", num_pages, num_trials, ns_per_access);

    free(base);

    return 0;
}
