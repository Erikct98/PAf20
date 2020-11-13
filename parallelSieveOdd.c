#include "sieve.h"

#define GOLDBACH_CONJECTURE

static unsigned int P;

void parallelSieve() {
    double startTime, endTime;

    long N = (S + 1) / 2;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    if (pid == 0) startTime = bsp_time();

    /** Setup sieve **/
    long p = bsp_nprocs();
    long fsqrtS = intSqrt(S, true);
    long start_size = max(fsqrtS + 1, (N + p - 1) / p);
    long size, intervalStart = 0;
    if (pid == 0) {
        size = start_size;
    } else {
        size = (N - start_size) / (p - 1);
        long extra = (N - start_size) % (p - 1);
        if (extra > pid - 1) size++;
        intervalStart = start_size + (pid - 1) * size + min(extra, pid - 1);
    }
    long firstValue = 2 * intervalStart + 1;
    long lastValue = 2 * (intervalStart + size - 1) + 1;
    bool *Vec = vecallocb(size);
    bsp_push_reg(Vec, size * sizeof(bool));
    memset(Vec, true, size);

    // skip 1
    if (intervalStart == 0) {
        Vec[0] = false;
    }

    // Setup nextPrime field
    long NextPrime = -1;
    bsp_push_reg(&NextPrime, sizeof(long));
    bsp_sync();

    // Run sieve
    long primeIndex = 0;
    while (true) {
        if (pid == 0) {
            primeIndex ++;

            // Have processor 0 find the next prime
            while (!Vec[primeIndex]) {
                primeIndex++;
            }
            // Prepare to stop if large smallest prime is greater than sqrt(S)
            if ((2 * primeIndex + 1) > fsqrtS) {
                primeIndex = -1;
            }
            // Communicate next prime to other processes
            long actualPrime = primeIndex * 2 + 1;
            for (long i = 0; i < P; i++) {
                bsp_put(i, &actualPrime, &NextPrime, 0, sizeof(long));
            }
        }

        // Synchronize
        bsp_sync();

        // Quit if we have found all primes
        if (NextPrime < 0) break;

        long diff = NextPrime - (firstValue % NextPrime);
        long startIndex;
        if (diff == NextPrime) {
            // first value is diveded by prime
            startIndex = 0;
        } else if (diff % 2 != 0) {
            // not but taking odd amount of steps until next encounter
            startIndex = (diff + NextPrime) / 2;
        } else {
            // taking even amount of steps
            startIndex = diff / 2;
        }

        long startValue = 2 * (intervalStart + startIndex) + 1;
        if (2 * startIndex + 1 <= NextPrime) {
            startIndex += NextPrime;
        }

        for (long j = startIndex; j < size; j += NextPrime) {
            Vec[j] = false;
        }
    }

    // Determine end time
    if (pid == 0) endTime = bsp_time();

#ifndef GOLDBACH_CONJECTURE

    // Report primes
//    for (long i = 0; i < P; i++) {
//        if (pid == i) {
//            if (pid == 0) {
//                printf("%ld, ", 2l);
//            }
//            for (long j = firstValue; j <= lastValue; j+= 2)
//                if (Vec[j / 2 - intervalStart]) printf("%ld, ", j); // Translate local vector state to actual primes
//        }
//        bsp_sync();
//    }

// Report primes
    for (long i = 0; i < P; i++) {
        if (pid == i) {
            long begin = pid == 0 ? 2 : 0;
//            for (long j = begin; j < size; j++)
//                if (Vec[j]) printf("%ld, ", intervalStart + j); // Translate local vector state to actual primes
            for (long j = lastValue; j >= firstValue; j-= 2)
                if (Vec[j / 2 - intervalStart]) {
                    printf("%ld, ", j); // Translate local vector state to actual primes
                    break;
                }
        }
        bsp_sync();
    }

#else
    bsp_sync();


    bool* allPrimes = vecallocb(N);
    bsp_push_reg(allPrimes, N * sizeof(bool));

    // Release memory
    bsp_pop_reg(&NextPrime);

    bsp_sync();

    for (long t = 0; t < p; t++) {
//        printf("%ld putting %ld with offset=%ld, size=%ld, 0x%x\n", pid, t, intervalStart, size, allPrimes);
        bsp_put(t, Vec, allPrimes, intervalStart * sizeof(bool), size * sizeof(bool));
    }

    bsp_pop_reg(Vec);
    vecfreeb(Vec);

    bsp_sync();

    // we do not actually know 2 is prime so we skip 4

    long counterExample = -1;

    for (long n = 4 + 2 * (pid); n < S + 1; n += 2 * (p - 1)) {
        bool found = false;
        for (long i = 0; i < (N + 1) / 2; ++i) {
            if (allPrimes[i]) {
                if (allPrimes[(n - (i * 2 + 1)) / 2]) {
#ifdef OUTPUT_GOLDBACH
                    printf("A sum for: %ld=%ld + %ld\n", n, i * 2 + 1, n - (i * 2 + 1));
#endif
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            counterExample = n;
            break;
        }
    }

    bsp_sync();

    if (counterExample > 0) {
        printf("%ld found counter-example: %ld\n", pid, counterExample);
    } else {
        printf("%ld found no counter example up to: %ld\n", pid, S + 1 );
    }


    bsp_pop_reg(allPrimes);
    vecfreeb(allPrimes);

#endif

#ifndef GOLDBACH_CONJECTURE
    // Release memory
    bsp_pop_reg(&NextPrime);

    bsp_pop_reg(Vec);
    vecfreeb(Vec);

#endif

    // Report running time
    if (pid == 0) printf("\nWith %ld procs Time: %f\n", p, S, endTime - startTime);

    bsp_end();
}

int main(int argc, char **argv) {
    if (argc > 1) {
        P = atoi(argv[1]);
    } else {
        printf("How many processors do you want to use?\n");
        fflush(stdout);
        scanf("%u", &P);
    }

    if (P == 0 || P > bsp_nprocs()) {
        fprintf(stderr, "Cannot use %u processors.\n", P);
//        return -1;
    }

    if (argc > 2) {
        S = atoll(argv[2]);
//        printf("running to %ld (%s)", S, argv[2]);
    }

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}

//2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997