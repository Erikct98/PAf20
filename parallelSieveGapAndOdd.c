#include "sieve.h"

static unsigned int P;

/*
 * Returns the first multiple of p that is greater than
 * or equal to nr
 */
long nextMultiple(int p, int nr){
    return -(p+nr) % p + nr;
}

long getIndex(long n){
    return (n - 1) / 2;
}

long getNumber(long index){
    return 2 * index + 1;
}

long getNumberX(long p, long idx, long intervalStart){
    return getNumber(intervalStart + idx * (p-1));
}

void parallelSieveGapAndOdd() {
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    if (pid == 0) startTime = bsp_time();

    // Compute some useful variable
    int sIdx = getIndex(S);
    int fsqrtSIdx = getIndex(intSqrt(S, true));

    // Setup sieve
    long p = bsp_nprocs();
    long vecSize, vecStart;

    long baseVecSize = max(fsqrtSIdx + 2, ceildiv(sIdx, p));;
    if (pid == 0){
        vecStart = 0;
        vecSize = baseVecSize;
    } else {
        vecStart = baseVecSize + (pid - 1);
        vecSize = ceildiv(sIdx - vecStart, p-1);
    }
    bool *Vec = vecallocb(vecSize);
    bsp_push_reg(Vec, vecSize * sizeof(bool));
    for (int i = 0; i < vecSize; i++) Vec[i] = true;


    // Setup nextPrimeIdx
    long nextPrimeIdx = 0;
    bsp_push_reg(&nextPrimeIdx, sizeof(long));

    bsp_sync();
    long primeIdx = 0;
    while(true){
        // Have pid = 0 calculate the next prime
        if (pid == 0){
            primeIdx ++;
            while (!Vec[primeIdx] && primeIdx < vecSize) primeIdx ++;
            nextPrimeIdx = primeIdx;
            for (int i = 1; i < p; i++){
                bsp_put(i, &primeIdx, &nextPrimeIdx, 0, sizeof(long));
            }
        }

        bsp_sync();

        // Stop after reaching the end.
        if (nextPrimeIdx > fsqrtSIdx) {
            break;
        }

        // Sieve
        long prime = getNumber(nextPrimeIdx);
        long primeSqrIdx = nextPrimeIdx * (prime + 1);
        if (pid == 0){
            long startIndex = primeSqrIdx;
            for (int j = startIndex; j < vecSize; j += prime) Vec[j] = false;
        } else {
            long startIndex = max(0, ceildiv(primeSqrIdx - vecStart, p - 1));
            long startNumber = getNumber(vecStart + startIndex * (p-1));
            long steps = 0;
            while (startNumber % prime != 0 && steps < prime){
                startNumber += 2 * (p-1);
                steps ++;
            }
            if (steps < prime){
                startIndex += steps;
                long stepSize = prime / gcd(prime, p-1);
                for (int j = startIndex; j < vecSize; j += stepSize) Vec[j] = false;
            }
        }
    }

    // Determine end time
    if (pid == 0) endTime = bsp_time();

    // Report primes
//    for (int i = 0; i < p; i++) {
//        if (pid == i) {
//            long begin = pid == 0 ? 1 : 0;
//            for (long j = begin; j < vecSize; j++)
//                if (Vec[j]){
//                    long prime = pid == 0 ? getNumber(j) : getNumber(vecStart + j * (p-1));
//                    printf("%ld, ", prime); // Translate local vector state to actual primes
//                }
//        }
//        bsp_sync();
//    }

    // Clean up
    bsp_pop_reg(&nextPrimeIdx);
    bsp_pop_reg(Vec);
    vecfreeb(Vec);

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