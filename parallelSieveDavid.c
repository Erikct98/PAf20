#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "sieve.h"

static unsigned int P;

void parallelSieve() {
    /** Begin parallel processing **/
    bsp_begin(P);
    long p = bsp_nprocs();
    long pid = bsp_pid();
    long csqrtS = intSqrt((int) S, false);

//    printf("its ya boi: %ld\n", pid);

    double startTime = bsp_time();

    /** Setup sieve vector for all processes */
    // Have all processes determine the interval of their vector
    bool *is_prime;
    long start_size = max(csqrtS, (S + p - 1) / p);
    long size, intervalStart = 0;
    if (pid == 0) {
        size = start_size;
    } else {
        size = (S - start_size) / (p - 1);
        long extra = (S - start_size) % (p - 1);
        intervalStart = start_size + (pid - 1) * size + min(extra, (pid - 1));
        if (extra > pid - 1)
            size++;
    }
    is_prime = vecallocb(size);
    bsp_push_reg(is_prime, size *sizeof(bool));

    // Have all processes clean their vector
    for (int i = 0; i < size; i++) {
        is_prime[i] = true;
    }

    /** Setup nextPrime field for all processes **/
    const int nextPrimeSize = 2;
    long* nextPrimes = vecalloci(nextPrimeSize);
    bsp_push_reg(nextPrimes, nextPrimeSize * sizeof(long));
    for (int i = 0; i < nextPrimeSize; i++) {
        nextPrimes[i] = -1;
    }


    bool running = true;
    int currPrime = 0;
    bsp_sync();

    /** Sieving **/
    long primeIndex = 1;
    while (running) {
        primeIndex++;
        // Find next prime
        if (pid == 0) {
            while (currPrime < nextPrimeSize) {
                while (!is_prime[primeIndex]) {
                    primeIndex++;
                }

                // Prepare to stop if large smallest prime is greater than sqrt(S)
                if (primeIndex > csqrtS) {
                    primeIndex = -1;
                    while (currPrime < nextPrimeSize) {
                        nextPrimes[currPrime++] = primeIndex;
                    }
                    running = false;
                } else {
                    nextPrimes[currPrime++] = primeIndex;
                }

                if (currPrime >= nextPrimeSize) {
                    for (int i = 0; i < P; i++) {
                        bsp_put(i, nextPrimes, nextPrimes, 0, nextPrimeSize * sizeof(long));
                    }
                } else {
                    long startIndex = max((primeIndex - intervalStart % primeIndex) % primeIndex,
                                          primeIndex * primeIndex - intervalStart);
                    for (int j = startIndex; j < size; j += primeIndex) {
                        is_prime[j] = false;
                    }
                }
            }
            currPrime = 0;

            // Communicate next prime other processes
        }

        bsp_sync();

//        printf("Ya boi %ld got: %ld, ... , %ld\n", pid, nextPrimes[0], nextPrimes[nextPrimeSize - 1]);

        // Quit if we have found all primes
        if (nextPrimes[0] < 0) {
            break;
        }

        if (pid == 0 && primeIndex > 0) {
            long startIndex = max((primeIndex - intervalStart % primeIndex) % primeIndex,
                                  primeIndex * primeIndex - intervalStart);
            for (int j = startIndex; j < size; j += primeIndex) {
                is_prime[j] = false;
            }
        } else if (pid != 0) {
            // Remove all multiples of the prime
            for (int i = 0; i < nextPrimeSize; ++i) {
                long prime = nextPrimes[i];
//                printf("pid: %ld striping: %ld\n", pid, prime);
                if (prime < 0) {
                    running = false;
                    break;
                }
                long startIndex = max((prime - intervalStart % prime) % prime,
                                      prime * prime - intervalStart);
                for (int j = startIndex; j < size; j += prime) {
                    is_prime[j] = false;
                }
            }
        }

    }

    bsp_sync();

    /** Print found primes **/
    for (int i = 0; i < P; i++) {
        if (pid == i) {
            long begin = pid == 0 ? 2 : 0;
            for (long j = begin; j < size; j++)
                if (is_prime[j])
                    printf("%ld, ",
                           intervalStart +
                           j); // Translate local vector state to actual primes
        }
        bsp_sync();
    }

    double endTime = bsp_time();

    bsp_pop_reg(nextPrimes);
    vecfreei(nextPrimes);
    bsp_pop_reg(is_prime);
    vecfreeb(is_prime);

    bsp_end();

    printf("\nThat took: %2.5f\n", (endTime - startTime));

}

int main(int argc, char **argv) {
    // Ask for sieve length

//    sequentialSieve(1000 * 1000 * 1000);

    printf("How many processors do you want to use?\n");
    fflush(stdout);
    if (!scanf("%u", &P)) {
        fprintf(stderr, "Cannot read input.\n");
    }
    if (P == 0 || P > bsp_nprocs()) {
        fprintf(stderr, "Cannot use %u processors.\n", P);
    } else {
        printf("Can use %u processors\n", P);
    }

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}