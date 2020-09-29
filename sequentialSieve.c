#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>

// Allow for the use of booleans
typedef int bool;
#define true 1
#define false 0

static unsigned int P;
static const unsigned int S = 1000;

static int ceilSqrt(int x){
    // Base cases
    if (x == 0 || x == 1)
        return x;

    int start = 0;
    int end = x/2 + 1;
    while (start + 1 < end) {
        int center = (start + end)/2;
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
    int sqrtS = (ceilSqrt((int) 2*S) - 1)/2;
    printf("Found upper bound: %u\n", sqrtS);

    // Run sieve
    printf("Found a prime: %u\n", 2);
    for (int i = 1; i < sqrtS; i++){
        if (sieve[i] == true){
            int p = 2 * i + 1;
            printf("Found a prime: %u\n", p);
            for (int j = i * (p+1); j < S; j += p){
                sieve[j] = false;
            }
        }
    }
    for (int i = sqrtS; i < S; i++){
        if (sieve[i] == true){
            int p = 2 * i + 1;
            printf("Found a prime: %u\n", p);
        }
    }
}

int main(int argc, char ** argv){
    // Ask for sieve length
    printf("How many processors do you want to use?\n");
    fflush( stdout );
    scanf ("%u", &P);
    if (P == 0 || P > bsp_nprocs()){
        fprintf(stderr, "Cannot use %u processors.\n", P);
    }

    bsp_init(&sequentialSieve, argc, argv);
    sequentialSieve();
    return EXIT_SUCCESS;
}