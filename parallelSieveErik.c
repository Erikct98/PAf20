#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "sequentialSieve.h"

static unsigned int P;
static const unsigned long S = 1000 * 1000 * 1000;

void parallelSieve() {
    /** Begin parallel processing **/
    bsp_begin(P);
    long p = bsp_nprocs();
    long pid = bsp_pid();
    long csqrtS = ceilSqrt((int) S);

    double startTime = bsp_time();

    /** Setup sieve vector for all processes */
    // Have all processes determine the interval of their vector
    bool *Vec;
    long start_size = max(csqrtS, (S + p - 1) / p);
    long size, intervalStart = 0;
    if (pid == 0) {
        size = start_size;
    } else {
        size = (S - start_size) / (p - 1);
        long extra = (S - start_size) % (p - 1);
        intervalStart = start_size + (pid - 1) * size + min(extra, (pid - 1));
        if (extra > pid - 1) size++;
    }
    Vec = vecallocb(size);
    bsp_push_reg(Vec, size * sizeof(bool));

    // Have all processes clean their vector
    for (int i = 0; i < size; i++) {
        Vec[i] = true;
    }

    /** Setup nextPrime field for all processes **/
    long NextPrime = -1;
    bsp_push_reg(&NextPrime, sizeof(long));
    bsp_sync();

    /** Sieving **/
    long primeIndex = 1;
    while (primeIndex <= csqrtS) {
        primeIndex++;
        // Find next prime
        if (pid == 0) {
            while (!Vec[primeIndex]) {
                primeIndex++;
            }

            // Prepare to stop if large smallest prime is greater than sqrt(S)
            if (primeIndex > csqrtS) {
                primeIndex = -1;
            }

            // Communicate next prime other processes
            for (int i = 0; i < P; i++) {
                bsp_put(i, &primeIndex, &NextPrime, 0, sizeof(long));
            }
        }

        bsp_sync();

        // Quit if we have found all primes
        if (NextPrime < 0) {
            break;
        }

        // Remove all multiples of the prime
        long startIndex = max(
                (NextPrime - intervalStart % NextPrime) % NextPrime,
                NextPrime * NextPrime - intervalStart);
        for (int j = startIndex; j < size; j += NextPrime) {
            Vec[j] = false;
        }
    }

    double endTime = bsp_time();

    /** Print found primes **/
    for (int i = 0; i < P; i++) {
        if (pid == i) {
            long begin = pid == 0 ? 2 : 0;
            for (long j = begin; j < size; j++)
                if (Vec[j]) printf("%ld, ", intervalStart + j); // Translate local vector state to actual primes
        }
        bsp_sync();
    }

    bsp_end();

    printf("\nThat took: %2.5f\n", (endTime - startTime));
}

int main(int argc, char **argv) {
    // Ask for sieve length
    printf("How many processors do you want to use?\n");
    fflush(stdout);
    scanf("%u", &P);
    if (P == 0 || P > bsp_nprocs()) {
        fprintf(stderr, "Cannot use %u processors.\n", P);
    }

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}