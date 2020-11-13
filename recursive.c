#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int N;

bool interference(int *board, int idx) {
    int idxHeight = board[idx];
    for (int i = 0; i < idx; i++){
        int horDiff = idx - i;
        int verDiff = abs(idxHeight - board[i]);
        if (horDiff == verDiff) return true;
    }
    return false;
}

bool recursiveCall(int *board, bool *used, int idx);

bool recursiveCall(int *board, bool *used, int idx) {
    if (idx == N) {
        return true;
    }

    for (int i = 0; i < N; i++) {
        if (!used[i]) {
            board[idx] = i;
            used[i] = true;
            if (!interference(board, idx) && recursiveCall(board, used, idx + 1)) {
                return true;
            }
            used[i] = false;
        }
    }
    return false;
}

void recursiveSolve() {
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();

    // Setup sieve
    int *board = malloc(N * sizeof(int));
    bool *used = malloc(N * sizeof(bool));
    for (int i = 0; i < N; i++) board[i] = -1;

    // Start recursive call
    bool foundSolution = recursiveCall(board, used, 0);

    // End timing
    endTime = clock();

    // Print result
    if (!foundSolution) {
        printf("Failed to find a solution!\n");
    } else {
        printBoard(N, board);
    }

    free(board);
    free(used);

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