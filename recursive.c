#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int N;

void recursiveSolve() {
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();



    endTime = clock();
    // Report running time
    clock_t time_taken = (endTime - startTime);
    printf("Solving %d-queens took recursiveSolve: %f\n", N, (double) time_taken / CLOCKS_PER_SEC);
}


int main(int argc, char ** argv) {
    if (argc > 1) {
        N = atoi(argv[1]);
    } else {
        printf("What size chessboard do we use?\n");
        fflush(stdout);
        if (scanf("%u", &N) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
    }

    recursiveSolve();

    return EXIT_SUCCESS;
}
