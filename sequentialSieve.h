#ifndef PAF20_PARALLELALGORITHMS_SEQUENTIALSIEVE_H
#define PAF20_PARALLELALGORITHMS_SEQUENTIALSIEVE_H

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include "../BSPedupack2.0/bspedupack.h"

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

long min(long one, long two){
    return one < two ? one : two;
}

long max(long one, long two){
    return one > two ? one : two;
}

void sequentialSieve(long S){
    bool sieve[S];

    // Assume all are prime
    for (int i = 0; i < S; i++){
        sieve[i] = true;
    }

    // Compute upper bound
    long sqrtS = (ceilSqrt((int) 2*S) - 1)/2;
//    printf("Found upper bound: %ld\n", sqrtS);

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
    printf("2, ");
    for (long i = 1; i < S; i++){
        if (sieve[i]){
            long p = 2 * i + 1;
            printf("Found a prime: %ld\n", p);
        }
    }
}

#endif //PAF20_PARALLELALGORITHMS_SEQUENTIALSIEVE_H