/* tlb_probe_estimate.c
 *
 * POSIX‐only TLB‐probe sweep + automatic TLB‐size estimate.
 *
 * Build:
 *   cc -std=c11 -O2 -Wall tlb_probe_estimate.c -o tlb_probe_estimate
 *
 * Run:
 *   ./tlb_probe_estimate > tlb_data.txt
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> /* gettimeofday() */
#include <time.h>     /* time(), srand(), rand() */
#include <unistd.h>   /* sysconf(), posix_memalign() */

static double measure_ns_per_access(int pages, long trials) {
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size < 0)
        page_size = 4096;
    size_t ips = page_size / sizeof(int);
    size_t total = (size_t)pages * ips;

    int *buffer;
    if (posix_memalign((void **)&buffer, page_size, total * sizeof *buffer) != 0) {
        perror("posix_memalign");
        exit(1);
    }

    /* fault each page in */
    for (size_t i = 0; i < total; i += ips)
        buffer[i] = 0;

    /* random in-page offsets */
    size_t *offs = malloc(pages * sizeof *offs);
    if (!offs) {
        perror("malloc");
        exit(1);
    }
    srand((unsigned)time(NULL) ^ (unsigned)pages);
    for (int p = 0; p < pages; p++)
        offs[p] = rand() % ips;

    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    for (long t = 0; t < trials; t++) {
        for (int p = 0; p < pages; p++) {
            size_t idx = (size_t)p * ips + offs[p];
            buffer[idx] += 1;
        }
    }

    gettimeofday(&t1, NULL);
    free(offs);
    free(buffer);

    /* elapsed ns */
    long sec = t1.tv_sec - t0.tv_sec;
    long usec = t1.tv_usec - t0.tv_usec;
    if (usec < 0) {
        sec--;
        usec += 1000000;
    }
    double elapsed_ns = (double)sec * 1e9 + (double)usec * 1e3;

    return elapsed_ns / ((double)trials * pages);
}

/* binary search between low_pages and high_pages for the “knee” point */
static int estimate_tlb(int low_pages, int high_pages, double t_base, double t_slow, long base_trials) {
    /* midpoint threshold */
    double threshold = (t_base + t_slow) / 2.0;

    while (high_pages - low_pages > 1) {
        int mid = (low_pages + high_pages) / 2;
        long trials = base_trials / mid;
        if (trials < 1)
            trials = 1;

        double t_mid = measure_ns_per_access(mid, trials);
        if (t_mid < threshold)
            low_pages = mid;
        else
            high_pages = mid;
    }
    return low_pages;
}

int main(void) {
    const int page_counts[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
    const int N = sizeof page_counts / sizeof *page_counts;
    const long BASE_TRIALS = 100000000L;

    printf("# %6s %12s %12s\n", "pages", "trials", "ns/access");

    /* store sweep results */
    double results[N];
    for (int i = 0; i < N; i++) {
        int pages = page_counts[i];
        long trials = BASE_TRIALS / pages;
        if (trials < 1)
            trials = 1;
        results[i] = measure_ns_per_access(pages, trials);
        printf("%8d %12ld %12.2f\n", pages, trials, results[i]);
    }

    /* find the biggest jump in ns/access */
    int idx = 0;
    double max_diff = 0.0;
    for (int i = 0; i < N - 1; i++) {
        double diff = results[i + 1] - results[i];
        if (diff > max_diff) {
            max_diff = diff;
            idx = i;
        }
    }

    int low_pages = page_counts[idx];
    int high_pages = page_counts[idx + 1];
    double t_base = results[idx];
    double t_slow = results[idx + 1];

    int est = estimate_tlb(low_pages, high_pages, t_base, t_slow, BASE_TRIALS);

    printf("\n# Detected largest jump: %d → %d pages (Δ=%.2f ns)\n", low_pages, high_pages, max_diff);
    printf("# Estimated TLB size ≈ %d pages\n", est);

    return 0;
}
