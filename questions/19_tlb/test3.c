/* tlb_probe_sweep.c
 *
 * Portable POSIX-only TLB-probe sweep:
 *   - pages = 1,2,4…2048
 *   - trials = 100,000,000/pages (min 1)
 *   - random in-page offsets to avoid L1 set thrash
 *   - measures ns/access via gettimeofday()
 *   - prints pages, trials, ns/access, cycles/access (≈ ns × 4)
 *
 * Build:
 *   cc -std=c11 -O2 -Wall tlb_probe_sweep.c -o tlb_probe_sweep
 *
 * Run:
 *   ./tlb_probe_sweep > tlb_data.txt
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> /* gettimeofday() */
#include <time.h>     /* time(), srand(), rand() */
#include <unistd.h>   /* sysconf(), posix_memalign() */

static double measure_ns_per_access(int num_pages, long num_trials) {
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size < 0) {
        perror("sysconf");
        exit(1);
    }
    size_t ints_per_page = page_size / sizeof(int);
    size_t total_ints = (size_t)num_pages * ints_per_page;

    /* allocate page‐aligned buffer */
    int *array;
    if (posix_memalign((void **)&array, page_size, total_ints * sizeof *array) != 0) {
        fprintf(stderr, "ERROR: alloc %d pages\n", num_pages);
        exit(1);
    }

    /* fault each page in */
    for (size_t i = 0; i < total_ints; i += ints_per_page)
        array[i] = 0;

    /* random in-page offsets */
    size_t *offsets = malloc(num_pages * sizeof *offsets);
    if (!offsets) {
        perror("malloc");
        exit(1);
    }
    srand((unsigned)time(NULL) ^ (unsigned)num_pages);
    for (int p = 0; p < num_pages; p++)
        offsets[p] = (size_t)(rand() % ints_per_page);

    /* time the loop */
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);

    for (long t = 0; t < num_trials; t++) {
        for (int p = 0; p < num_pages; p++) {
            size_t idx = (size_t)p * ints_per_page + offsets[p];
            array[idx] += 1;
        }
    }

    gettimeofday(&tv_end, NULL);
    free(offsets);
    free(array);

    /* compute elapsed microseconds */
    long sec = tv_end.tv_sec - tv_start.tv_sec;
    long usec = tv_end.tv_usec - tv_start.tv_usec;
    if (usec < 0) {
        sec -= 1;
        usec += 1000000;
    }
    double elapsed_us = (double)sec * 1e6 + (double)usec;

    /* return ns per access */
    double total_accesses = (double)num_trials * num_pages;
    return (elapsed_us * 1e3) / total_accesses;
}

int main(void) {
    const int page_counts[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    const int N = sizeof page_counts / sizeof *page_counts;
    const long BASE_TRIALS = 100000000L;
    const double CPU_GHZ = 4.0;           /* default 4 GHz */
    const double CYCLES_PER_NS = CPU_GHZ; /* 4 cycles/ns */

    /* Header */
    printf("# %6s %12s %12s %16s\n", "pages", "trials", "ns/access", "cycles/access");

    for (int i = 0; i < N; i++) {
        int pages = page_counts[i];
        long trials = BASE_TRIALS / pages;
        if (trials < 1)
            trials = 1;

        double ns = measure_ns_per_access(pages, trials);
        double cyc = ns * CYCLES_PER_NS;

        printf("%8d %12ld %12.2f %16.1f\n", pages, trials, ns, cyc);
    }

    return 0;
}
