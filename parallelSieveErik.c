#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "sequentialSieve.h"
//#include "../BSPedupack2.0/bspedupack.h"

static unsigned int P;
static const unsigned long S = 1000;

void parallelSieve() {
    /** Begin parallel processing **/
    bsp_begin(P);
    long p = bsp_nprocs();
    long pid = bsp_pid();
    long csqrtS = ceilSqrt((int) S);

    /** Setup sieve part for all processes */
    // Have all processes determine the interval of their vector
    bool *Vec;
    long start_size = max(csqrtS, (S + p - 1) / p);
    long size, intervalStart = 0, intervalEnd;
    if (pid == 0) {
        size = start_size;
        intervalEnd = size;
    } else {
        size = (S - start_size) / p;
        long extra = S - size * p;
        intervalStart = start_size + pid * size + min(extra, pid);
        if (extra > pid) size++;
        intervalEnd = intervalStart + size;
    }
    Vec = vecallocb(size);
    bsp_push_reg(Vec, size * sizeof(bool));

    printf("p%ld: start, end = %ld, %ld.\n", pid, intervalStart, intervalEnd);



//    /** Setup nextPrime field for all processes **/
//    long NextPrime = -1;
//    bsp_push_reg(NextPrime, sizeof(long));
//
//    bsp_sync();
//
//    /** Sieving **/
//
//    // Have all processes clean their vector
//    for (int i = 0; i < size; i++) {
//        Vec[i] = true;
//    }
//
//    long primeIndex = 2;
//    while (primeIndex < csqrtS) {
//        // Find next prime
//        if (pid == 0) {
//            while (!Vec[primeIndex]) {
//                primeIndex++;
//            }
//            // Communicate next prime other processes
//            for (int i = 0; i < P; i++) {
//                bsp_put(i, &primeIndex, &NextPrime, 0, sizeof(long));
//            }
//        }
//
//        bsp_sync();
//
//        // Remove all multiples of prime
//
//
//
//
//    }

    bsp_end();
}

int main(int argc, char ** argv){
    // Ask for sieve length
    printf("How many processors do you want to use?\n");
    fflush( stdout );
    scanf ("%u", &P);
    if (P == 0 || P > bsp_nprocs()){
        fprintf(stderr, "Cannot use %u processors.\n", P);
    }

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}