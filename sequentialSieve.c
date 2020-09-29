#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "../BSPedupack2.0/bspedupack.h"

static unsigned int P;
static const unsigned int S = 1000;

static long ceilSqrt(int x){
    // Base cases
    if (x == 0 || x == 1)
        return x;

    long start = 0;
    long end = x/2 + 1;
    while (start + 1 < end) {
        long center = (start + end)/2;
        if (center * center > x){
            end = center;
        } else {
            start = center;
        }
    }
    return end;
}

void sequentialSieve(){
    bool sieve[S];

    // Assume all are prime
    for (int i = 0; i < S; i++){
        sieve[i] = true;
    }

    // Compute upper bound
    long sqrtS = (ceilSqrt((int) 2*S) - 1)/2;
    printf("Found upper bound: %ld\n", sqrtS);

    // Run sieve
    printf("Found a prime: %u\n", 2);
    for (long i = 1; i < sqrtS; i++){
        if (sieve[i]){
            long p = 2 * i + 1;
            printf("Found a prime: %ld\n", p);
            for (long j = i * (p+1); j < S; j += p){
                sieve[j] = false;
            }
        }
    }
    for (long i = sqrtS; i < S; i++){
        if (sieve[i]){
            long p = 2 * i + 1;
            printf("Found a prime: %ld\n", p);
        }
    }
}

long min(long one, long other){
    return one < other ? one : other;
}

void parallelSieve(){
    // TODO: place this somewhere better!
    static bool sieve[S];
    memset(sieve, true, S*sizeof(bool));

    /** Begin parallel processing **/
    bsp_begin(P);
    long procs = bsp_nprocs();
    long s = bsp_pid();
    long csqrtS = ceilSqrt((int) S);

    /** Prep sieve */
    long start = 1;
    while (start * (2 * start + 2) - (2 * start + 1) < procs) {
        start ++;
        if (s == 0 && sieve[start]){
            long p = 2 * start + 1;
            for (long j = start + p; j < S; j += p){
                sieve[j] = false;
            }
        }
    }

    printf("I, process %ld, have found start to be %ld.\n", s, start);
    bsp_sync();

    while (start < S){
        long startNr = 2 * start + 1;
        long endIdx = start * (startNr + 1);

        for (long i = start + s; i < min(csqrtS, endIdx); i += procs){
            printf("p%ld: checking %ld.\n", s, i);
            if (sieve[i]) {
                long p = 2 * i + 1;
                printf("I, process %ld, found %ld to be a prime.\n", s, p);
                for (long j = i * (p+1); j < S; j += p){
//                    printf("p%ld: knocking over %ld.\n", s, j);
                    sieve[j] = false;
                }
            }
        }

        start = endIdx;

        /**** Synchronize between processes ****/
        bsp_sync();

        if (s == 0){
            printf("\n[synchronizing...]\n\n");
        }
        bsp_sync();
    }

    if (s == 0){
        for (long i = 1; i < S; i += 1){
            if (sieve[i]) {
                long p = 2 * i + 1;
                printf("%ld, ", p);
            }
        }
        printf("\n");
    }
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

//    bsp_init(&sequentialSieve, argc, argv);
//    sequentialSieve();

    bsp_init(&parallelSieve, argc, argv);
    parallelSieve();

    return EXIT_SUCCESS;
}