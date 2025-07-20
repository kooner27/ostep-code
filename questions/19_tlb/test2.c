/* tlb_probe.c  —  measure per-access cost while varying #pages
 *
 * 1. vary the offset inside each successive page so we don’t pin all
 *    accesses to cache-set 0 (the root cause of the 8-page “cliff”).
 * 2. count cycles with rdtscp+lfence (≈ 0.3 ns on modern x86-64),
 *    then print cycles/access.  convert to ns by dividing by your cpu’s
 *    MHz if you like, or let gnuplot do it.
 *
 * gcc -O2 -march=native -std=c11 tlb_probe.c -o tlb_probe
 */

#define _GNU_SOURCE  // for cpu_set_t and sched_setaffinity

#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // for sysconf()
#include <x86intrin.h>  // for __rdtscp() and lfence intrinsic

/* ------------------------------------------------------------------ */
/* helpers                                                            */
static inline uint64_t rdtscp(void) {
    unsigned aux;
    __builtin_ia32_lfence();
    uint64_t t = __rdtscp(&aux);
    __builtin_ia32_lfence();
    return t;
}

/* ------------------------------------------------------------------ */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <num_pages> <num_trials>\n", argv[0]);
        return 1;
    }
    const int pages = atoi(argv[1]);
    const long trials = atol(argv[2]);

    const size_t page_sz = sysconf(_SC_PAGESIZE);        // 4096 on x86
    const size_t ints_per_page = page_sz / sizeof(int);  // 1024
    const size_t total_ints = (size_t)pages * ints_per_page;

    int *buf;
    if (posix_memalign((void **)&buf, page_sz, total_ints * sizeof *buf)) {
        perror("posix_memalign");
        return 1;
    }

    volatile int *a = buf;  // keep accesses alive
    for (size_t i = 0; i < total_ints; i += ints_per_page)
        a[i] = 0;  // touch each page

    // Pin to CPU 0 to avoid cross‐core noise
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    sched_setaffinity(0, sizeof set, &set);

    /* ------------------------------------------------------------------
       main probe loop
       each page p:  index = p*ints_per_page + (p*16) % ints_per_page
       the (p*16) term walks through different cache lines, so bits 6–11
       vary and we don’t evict ourselves from one L1 cache set.
    */
    const size_t line_stride = 64 / sizeof(int);  // 64-byte cache lines

    // take timestamps
    uint64_t t0 = rdtscp();
    for (long t = 0; t < trials; ++t) {
        size_t off = 0;
        for (int p = 0; p < pages; ++p) {
            size_t idx = (size_t)p * ints_per_page + (off & (ints_per_page - 1));
            (void)a[idx];
            off += line_stride;
        }
    }
    uint64_t t1 = rdtscp();

    double cycles_total = (double)(t1 - t0);
    double cycles_access = cycles_total / (pages * trials);

    printf("%d\t%ld\t%.1f cycles/access\n", pages, trials, cycles_access);

    free(buf);
    return 0;
}
