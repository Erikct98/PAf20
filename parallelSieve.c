#include "sieve.h"

static unsigned int P;

void parallelSieve() {
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    if (pid == 0) startTime = bsp_time();

    /** Setup sieve **/
    long p = bsp_nprocs();
    long fsqrtS = intSqrt(S, true);
    long start_size = max(fsqrtS + 1, (S + p - 1) / p);
    long size, intervalStart = 0;
    if (pid == 0) {
        size = start_size;
    } else {
        size = (S - start_size) / (p - 1);
        long extra = (S - start_size) % (p - 1);
        if (extra > pid - 1) size++;
        intervalStart = start_size + (pid - 1) * size + min(extra, pid - 1);
    }
    bool *Vec = vecallocb(size);
    bsp_push_reg(Vec, size * sizeof(bool));
    for (int i = 0; i < size; i++) {
        Vec[i] = true;
    }

    // Setup nextPrime field
    long NextPrime = -1;
    bsp_push_reg(&NextPrime, sizeof(long));
    bsp_sync();

    // Run sieve
    long primeIndex = 1;
    while (true) {
        if (pid == 0) {
            primeIndex++;

            // Have processor 0 find the next prime
            while (!Vec[primeIndex]) {
                primeIndex++;
            }
            // Prepare to stop if large smallest prime is greater than sqrt(S)
            if (primeIndex > fsqrtS) {
                primeIndex = -1;
            }
            // Communicate next prime to other processes
            for (int i = 0; i < P; i++) {
                bsp_put(i, &primeIndex, &NextPrime, 0, sizeof(long));
            }
        }

        // Synchronize
        bsp_sync();

        // Quit if we have found all primes
        if (NextPrime < 0) break;

        // Remove all multiples of the prime
        long startIndex = max(
                (NextPrime - intervalStart % NextPrime) % NextPrime,
                NextPrime * NextPrime - intervalStart);
        for (int j = startIndex; j < size; j += NextPrime) {
            Vec[j] = false;
        }
    }

    // Determine end time
    if (pid == 0) endTime = bsp_time();

    // Report primes
    for (int i = 0; i < P; i++) {
        if (pid == i) {
            long begin = pid == 0 ? 2 : 0;
//            for (long j = begin; j < size; j++)
//                if (Vec[j]) printf("%ld, ", intervalStart + j); // Translate local vector state to actual primes
            for (long j = size - 1; j >= begin; --j) {
                if (Vec[j]) {
                    printf("%ld, ", intervalStart + j); // Translate local vector state to actual primes
                    break;
                }
            }
        }
        bsp_sync();
    }

    // Release memory
    bsp_pop_reg(&NextPrime);
    bsp_pop_reg(Vec);
    vecfreeb(Vec);

    // Report running time
    if (pid == 0) printf("\nParallel sieve with %ld procs up to %ld Time: %f\n", p, S, endTime - startTime);

    bsp_end();
}

int main(int argc, char **argv) {
    // Ask for sieve length

    if (argc > 1) {
        P = atoi(argv[1]);
    } else {
        printf("How many processors do you want to use?\n");
        fflush(stdout);
        scanf("%u", &P);
    }

    if (argc > 2) {
        S = atoll(argv[2]);
//        printf("running to %ld (%s)", S, argv[2]);
    }


    if (P == 0 || P > bsp_nprocs()) {
        fprintf(stderr, "Cannot use %u processors.\n", P);
        return -1;
    }

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}