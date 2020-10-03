#include "sieve.h"
#include <time.h>

/*
 * sequentialSieveBasic
 * Reports all primes in the range [2,S)
 * No fancy trickery
 */
void sequentialSieveBasic(){
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();

    // Setup sieve
    bool* sieve = malloc(S * sizeof(bool));
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
            printf("%ld,", i);
        }
    }

    // Clean up memory
    free(sieve);

    // Report running time
    endTime = clock();
    clock_t time_taken = (endTime - startTime);
    printf("\nsequentialSieveBasic time: %ld to %ld (%f)\n", startTime, endTime, (double)time_taken / CLOCKS_PER_SEC);
}

/*
 * sequentialSieveAdvanced
 * Reports all primes in the range [2,S)
 * Does not take multiples of two into account
 */
void sequentialSieveAdvanced(){
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();

    // Setup sieve
    bool *sieve = malloc(S/2 * sizeof(bool));
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
            printf("%ld,", p);
        }
    }

    // Clean up memory
    free(sieve);

    // Report running time
    endTime = clock();
    clock_t time_taken = (endTime - startTime);
    printf("\nsequentialSieveBasic time: %ld to %ld (%f)\n", startTime, endTime, (double)time_taken / CLOCKS_PER_SEC);
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
        sequentialSieveBasic();
    } else if (decide == 1){
        sequentialSieveAdvanced();
    }

    return EXIT_SUCCESS;
}