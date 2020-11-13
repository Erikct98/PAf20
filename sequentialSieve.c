#include "sieve.h"
#include <time.h>

/*
 * sequentialSieveBasic
 * Reports all primes in the range [2,S)
 * No fancy trickery
 */
void sequentialSieveBasic() {
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();

    // Setup sieve
    bool *sieve = malloc(S * sizeof(bool));
    for (long i = 0; i < S; i++) {
        sieve[i] = true;
    }

    // Compute upper bound
    long fqrtS = intSqrt(S, true);

    // Run sieve
    for (long i = 2; i <= fqrtS; i++) {
        if (sieve[i]) {
            for (long j = i * i; j < S; j += i) {
                sieve[j] = false;
            }
        }
    }

    // Determine end running time
    endTime = clock();

    // Report primes
    for (long i = S - 1; i >= 0; --i) {
        if (sieve[i]) {
            printf("%ld,", i);
            break;
        }
    }


    // Clean up memory
    free(sieve);

    // Report running time
    clock_t time_taken = (endTime - startTime);
    printf("\nTo %ld sequentialSieveBasic time: %f\n", S, (double) time_taken / CLOCKS_PER_SEC);
}

/*
 * sequentialSieveAdvanced
 * Reports all primes in the range [2,S)
 * Does not take multiples of two into account
 */
void sequentialSieveAdvanced() {
    clock_t startTime, endTime;


//    printf("0\n");


    // Start timing
    startTime = clock();

//    printf("0\n");

    // Setup sieve
    bool *sieve = malloc(S / 2 * sizeof(bool));
    if (sieve == NULL)
        exit(-1);

    for (long i = 0; i < S / 2; i++) {
        sieve[i] = true;
    }

    // Compute upper bound
    long fqrtS = (intSqrt(S, true) - 1) / 2;

    // Run sieve
    for (long i = 1; i <= fqrtS; i++) {
        if (sieve[i]) {
            long p = 2 * i + 1;
            for (long j = i * (p + 1); j < S / 2; j += p) {
                sieve[j] = false;
            }
        }
    }

    // Determine end running time
    endTime = clock();

    // Report primes
//    printf("%ld,", 2l);
//    for (long i = 1; i < S / 2; i++) {
//        if (sieve[i]) {
//            long p = 2 * i + 1;
//            printf("%ld,", p);
//        }
//    }
    for (long i = S / 2 - 1; i >= 0; --i) {
        if (sieve[i]) {
            printf("%ld,", 2 * i + 1);
            break;
        }
    }


    // Clean up memory
    free(sieve);

    // Report running time
    clock_t time_taken = (endTime - startTime);
    printf("\nTo %ld sequentialSieveAdvanced time: %f\n", S, (double) time_taken / CLOCKS_PER_SEC);
}

int main(int argc, char **argv) {
    int decide = 0;
    if (argc > 1) {
        decide = atoi(argv[1]);
    } else {
        printf("Which version do you want to run?\n");
        printf("0: sequentialSieveBasic\n");
        printf("1: sequentialSieveAdvanced\n");
        fflush(stdout);
        scanf("%u", &decide);
    }
    // Ask for sieve length

    if (argc > 2) {
        S = atoll(argv[2]);
//        printf("running to %ld (%s)", S, argv[2]);
    }

    if (decide < 0 || decide > 1) {
        fprintf(stderr, "Version %u does not exist.\n", decide);
    }

    if (decide == 0) {
        sequentialSieveBasic();
    } else if (decide == 1) {
        sequentialSieveAdvanced();
    }

    return EXIT_SUCCESS;
}