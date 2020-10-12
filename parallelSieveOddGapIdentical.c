#include "sieve.h"

//#define TWINPRIMES

static unsigned int P;
static long basicPrimes[5] = {3l,5l,7l,11l,13l};

long getIndex(long n) {
    return (n - 1) / 2;
}

long getNumber(long index) {
    return 2 * index + 1;
}

/*
 * Swaps the values of elements a and b.
 */
long swap(long *a, long *b){
    long temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Returns the multiplicative inverse of a mod n
 * or 0 if no inverse exists
 */
long inverse(long a, long n){
    long r = n, new_r = a;
    long t = 0, new_t = 1;

    while (new_r != 0) {
        long quotient = r / new_r;
        swap(&new_t, &t);
        new_t = new_t - quotient * t;

        swap(&new_r, &r);
        new_r = new_r - quotient * r;
    }
    return r > 1 ? 0 : (t + n) % n;
}

/*
 * Sets entries corresponding with all multiples of p greater than p^2 to false.
 * Note: this function assumes p > 1.
 */
long sievePrime(bool *sieve, long start, long gap, long size, long p, long P, long pid) {
//    printf("=================================\n");
//    printf("%ld, %ld, %ld, %ld, %ld.\n", pid, p, start, gap, P);

    // Determine first entry in sieve
    long primeIdx = getIndex(p); // Global index, not process specific
    long primeSqrIdx = primeIdx * (p + 1); // Global index, not process specific
    long offset = (pid - (primeSqrIdx % P) + P) % P; // Local offset, process specific
    long m_inv = inverse(p, gap); // Global TODO: have this only computed once by finder.
//    printf("%ld, %ld, %ld, %ld, %ld\n", p, primeIdx, primeSqrIdx, offset, m_inv);

    // Sieve primes
    long startIndex = size, stepSize = p / gcd(p, gap);
    if (m_inv > 0) {
        startIndex = (primeSqrIdx + ((offset * m_inv) % P) * p) / P; // local
    } else if (offset % p == 0) {
        startIndex = (primeSqrIdx + offset) / P; // local
    }
    for (int j = startIndex; j < size; j += stepSize) {
        sieve[j] = false;
//            printf("pid %ld: removing %ld\n", pid, getNumber(j * P + start));
    }
//    printf("=================================\n");
}

void parallelSieveGapAndOdd() {
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    if (pid == 0) startTime = bsp_time();

    /** SETUP PHASE **/

    // Compute some useful variables
    long gap = 2 * P;
    long sIdx = getIndex(S);
    long sieveStart = pid;
    long sieveSize = ceildiv(sIdx, P);
    long lastIndex = (getIndex(intSqrt(S, true)) - pid) / P + 1; // Local

    // Setup sieve
    bool *sieve = vecallocb(sieveSize);
    bsp_push_reg(sieve, sieveSize * sizeof(bool));
    for (int i = 0; i < sieveSize; i++){
        sieve[i] = true;
    }

    // Setup prime receive list
    long primesPerProc = P;
    long primesLength = P * primesPerProc;
    long *primes = vecalloci(primesLength); // each processor gets p slots
    bsp_push_reg(primes, primesLength * sizeof(long));
    long localOffset = pid * primesPerProc;


#ifdef TWINPRIMES
    // TODO: add twin prime support
#endif

    bsp_sync();

    /** SIEVING PHASE **/

    // Start setting up sieve by removing all primes in basicPrimes
    long nrBasicPrimes = sizeof(basicPrimes) / sizeof(long);
    for (int i = 0; i < nrBasicPrimes; i++) {
        sievePrime(sieve, sieveStart, gap, sieveSize, basicPrimes[i], P, pid);
    }

    // Determine chunk bounds
    long start = basicPrimes[nrBasicPrimes - 1] + 2;
    long startIdx = getIndex(start); // Global idx
    int chunkSize = start * startIdx; // Global size

    while (true) {
        // Find primes
        long primeIndex = localOffset;
        long startIndex = (startIdx - sieveStart + P - 1) / P; // Local idx
        long endIndex = min(lastIndex, (startIdx + chunkSize - sieveStart) / P); // Local idx
        for (int i = startIndex; i < endIndex && primeIndex < localOffset + primesPerProc; i++) {
            if (sieve[i]) primes[primeIndex++] = getNumber(sieveStart + P * i); // convert from local idx to prime
        }
        for (int i = primeIndex; i < localOffset + primesPerProc; i++){
            primes[i] = -1;
        }

        // Submit primes to neighbours
        for (int i = 1; i < P; i++) {
            bsp_put((pid + i) % P, &primes[localOffset], primes, localOffset * sizeof(long), primesPerProc * sizeof(long));
        }

        bsp_sync();

        // Check if we are done
        // FIXME: if this check fails, Goldbach conjecture is false FYI.
        bool allDone = true;
        for (int i = 0; i < primesLength; i++) {
            allDone &= primes[i] == -1;
        }
        if (allDone) break;

        // Remove multiples of primes
        for (int i = 0; i < primesLength; i++) {
            if (primes[i] != -1) sievePrime(sieve, sieveStart, gap, sieveSize, primes[i], P, pid);
        }

        // Update chunk bounds
        long smallest = startIdx + chunkSize;
        for (int i = primesPerProc - 1; i < primesLength; i += primesPerProc) {
            if (primes[i] != -1) smallest = min(smallest, primes[i]);
        }
        startIdx = smallest < startIdx + chunkSize ? getIndex(smallest) + 2 : startIdx + chunkSize;
        chunkSize = startIdx * getNumber(startIdx);

        // TODO: prevent reporting primes multiple times!
    }

    if (pid == 0) endTime = bsp_time();

#ifndef TWINPRIMES

    bsp_sync();

    // Report primes
    for (int i = 0; i < P; i++) {
        if (pid == i) {
//            printf("\n pid: %ld ---", pid);
            long begin = pid == 0 ? 1 : 0;
            for (long j = begin; j < sieveSize; j++)
//                printf("%d,", sieve[j]);
                if (sieve[j]) {
                    printf("%ld, ", getNumber(sieveStart + j * P)); // Convert local index to prime
                }
        }
        bsp_sync();
    }
#endif

    // Clean up
    bsp_pop_reg(primes);
    vecfreei(primes);
    bsp_pop_reg(sieve);
    vecfreeb(sieve);

#ifdef TWINPRIMES
    // TODO: implement twin primes
#endif

    // Report running time
    if (pid == 0) printf("\nTime: %f\n", endTime - startTime);

    bsp_end();
}

int main(int argc, char **argv) {
    // Ask for sieve length
    printf("How many processors do you want to use?\n");
    fflush(stdout);
    scanf("%u", &P);
    if (P == 0 || P > bsp_nprocs()) {
        fprintf(stderr, "Cannot use %u processors.\n", P);
    }

    printf("nr processes: %d\n", P);
    bsp_init(&parallelSieveGapAndOdd, argc, argv);
    parallelSieveGapAndOdd();

    return EXIT_SUCCESS;
}