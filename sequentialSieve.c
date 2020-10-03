#include "sieve.h"

/*
 * sequentialSieveBasic
 * Reports all primes in the range [2,S)
 * No fancy trickery
 */
void sequentialSieveBasic(){
    double startTime, endTime;

    // Start process
    bsp_begin(1);
    long pid = bsp_pid();

    // Start timing
    if (pid == 0) startTime = bsp_time();

    // Setup sieve
    bool *sieve = vecallocb(S);
    for (int i = 0; i < S; i++){
        sieve[i] = true;
    }

    // Compute upper bound
    long fqrtS = intSqrt(S, true);

    // Run sieve
    for (long i = 2; i <= fqrtS; i++){
        if (sieve[i]){
            for (long j = i * i; j < S; j += i){
                sieve[j] = false;
            }
        }
    }

    // Report primes
    for (long i = 1; i < S; i++){
        if (sieve[i]){
            printf("Found a prime: %ld\n", i);
        }
    }

    // Report running time
    if (pid == 0){
        endTime = bsp_time();
        printf("sequentialSieveBasic time: %f\n", endTime - startTime);
    }

    // Clean up memory
    vecfreeb(sieve);

    bsp_end();
}

/*
 * sequentialSieveAdvanced
 * Reports all primes in the range [2,S)
 * Does not take multiples of two into account
 */
void sequentialSieveAdvanced(){
    double startTime, endTime;

    // Start process
    bsp_begin(1);
    long pid = bsp_pid();

    // Start timing
    if (pid == 0) startTime = bsp_time();

    // Setup sieve
    bool *sieve = vecallocb(S/2);
    for (int i = 0; i < S/2; i++){
        sieve[i] = true;
    }

    // Compute upper bound
    long fqrtS = (intSqrt(S, true) - 1)/2;

    // Run sieve
    for (long i = 1; i <= fqrtS; i++){
        if (sieve[i]){
            long p = 2 * i + 1;
            for (long j = i * (p+1); j < S/2; j += p){
                sieve[j] = false;
            }
        }
    }

    // Report primes
    printf("Found a prime: %ld\n", 2l);
    for (long i = 1; i < S/2; i++){
        if (sieve[i]){
            long p = 2 * i + 1;
            printf("Found a prime: %ld\n", p);
        }
    }

    // Report running time
    if (pid == 0) {
        endTime = bsp_time();
        printf("sequentialSieveAdvanced time: %f\n", endTime - startTime);
    }

    // Clean up memory
    vecfreeb(sieve);

    bsp_end();
}

int main(int argc, char **argv) {
    int decide;
    // Ask for sieve length
    printf("Which version do you want to run?\n");
    printf("0: sequentialSieveBasic\n");
    printf("1: sequentialSieveAdvanced\n");
    fflush(stdout);
    scanf("%u", &decide);
    if (decide < 0 || decide > 1) {
        fprintf(stderr, "Version %u does not exist.\n", decide);
    }

    if (decide == 0){
        bsp_init(&sequentialSieveBasic, argc, argv);
        sequentialSieveBasic();
    } else if (decide == 1){
        bsp_init(&sequentialSieveAdvanced, argc, argv);
        sequentialSieveAdvanced();
    }

    return EXIT_SUCCESS;
}