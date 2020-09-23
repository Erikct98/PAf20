#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>

// Allow for the use of booleans
typedef int bool;
#define true 1
#define false 0

static unsigned int P;
static unsigned int S;

void sequentialSieve(){
    bool sieve[S];
    // Set all to zero
    for (int i = 0; i < S; i++){
        sieve[i] = false;
    }

    // Run sieve
    for (int i = 2; i < S; i++){
        if (sieve[i] == false){
            printf("Found a prime: %u\n", i);
            for (int j = i; j < S; j += i){
                sieve[j] = true;
            }
        }
    }
}

int main(int argc, char ** argv){
    // Ask for sieve length
    printf("What size sieve do you want to run?\n");
    fflush( stdout );
    scanf ("%u", &S);
    if (S < 3){
        fprintf(stderr, "Cannot make %u size sieve.\n", S);
    }

    bsp_init(&sequentialSieve, argc, argv);
    sequentialSieve();
    return EXIT_SUCCESS;
}