#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int N;

bool interference(int *board, int idx) {
    int idxHeight = board[idx];
    int horDiff = idx;
    for (int i = 0; i < idx; i++) {
        int verDiff = abs(idxHeight - board[i]);
        if (horDiff == verDiff) return true;
        horDiff--;
    }
    return false;
}

bool recursiveCall(int *board, int idx, int *remaining, int len);

bool recursiveCall(int *board, int idx, int *remaining, int len) {
    if (idx == N) return true;
    for (int i = 0; i < len; i++) {
        board[idx] = remaining[i];
        remaining[i] = remaining[len - 1];
        if (!interference(board, idx) && recursiveCall(board, idx + 1, remaining, len - 1)) {
            return true;
        }
        remaining[i] = board[idx];
    }
    return false;
}

void recursiveSolve() {
    clock_t startTime, endTime;

    // Start timing
    startTime = clock();

    // Setup sieve
    int *board = malloc(N * sizeof(int));
    if (!board) {
        printf("Malloc failed (board)");
        return;
    }
    int *remaining = malloc(N * sizeof(int));
    if (!remaining) {
        printf("Malloc failed (remaining)");
        return;
    }
    memset(board, -1, N * sizeof(int));
    for (int i = 0; i < N; i++) remaining[i] = i; // TODO: randomization -> randomize this initialization

    // Start recursive call
    bool foundSolution = recursiveCall(board, 0, remaining, N);

    // End timing
    endTime = clock();

    // Print result
    if (!foundSolution) {
        printf("Failed to find a solution!\n");
    } else {
        printBoard(N, board);
    }
    if (validBoard(N, board, N < 64)) {
        printf("Board is valid!\n");
    }

    free(board);
    free(remaining);

    // Report running time
    clock_t time_taken = (endTime - startTime);
    printf("Solving %d-queens took recursiveSolve: %f\n", N, (double) time_taken / CLOCKS_PER_SEC);
}


int main(int argc, char ** argv) {
    seedRandom();
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
