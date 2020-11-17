#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int P;
static int N;

bool diagonalInterference(int *board, int idx) {
    int idxHeight = board[idx];
    int horDiff = idx;
    for (int i = 0; i < idx; i++) {
        int verDiff = abs(idxHeight - board[i]);
        if (horDiff == verDiff) return true;
        horDiff--;
    }
    return false;
}

bool recursiveFindQueens(int *board, int idx, int *remaining, int len);

bool recursiveFindQueens(int *board, int idx, int *remaining, int len) {
    if (idx == N) return true;
    for (int i = 0; i < len; i++) {
        board[idx] = remaining[i];
        remaining[i] = remaining[len - 1];
        if (!diagonalInterference(board, idx) && recursiveFindQueens(board, idx + 1, remaining, len - 1)) {
            return true;
        }
        remaining[len - 1] = remaining[i];
        remaining[i] = board[idx];
    }
    return false;
}


void parallelQueens() {
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    startTime = bsp_time();

    /** Setup structs **/
    // Local
    int* board = vecallocint(N);
    int* remaining = vecallocint(N);

    /** Determine iteration depth **/
    long depth = 1, cases = N;
    while (P > cases) {
        cases *= N - depth;
        depth++;
    }

    /** Tackle assigned problems **/
    for (long i = pid; i < cases; i += P) {
        // Set remaining
        for (int j = 0; j < N; j++) remaining[j] = j; // TODO: randomization -> use a random permutation

        // Setup starting point for search
        int caseNr = i, idx;
        for (int j = 0; j < depth; j++) {
            idx = caseNr % (N-j);
            board[j] = remaining[idx];
            remaining[idx] = remaining[N - 1 - j];
            caseNr /= (N-j);
        }

        // Check if starting point is valid
        bool interference = false;
        for (int j = 1; j < depth; j++) {
            interference |= diagonalInterference(board, j);
        }
        if (interference) continue;

        // Attempt to find solution
        bool found = recursiveFindQueens(board, depth, remaining, N - depth);
        if (found) {
            fancyPrintBoard(N, board);
            break;
        }
    }

    endTime = bsp_time();

    printf("pid %ld - runtime %f\n", pid, endTime - startTime);

    vecfreeint(board);
    vecfreeint(remaining);
    bsp_end();
}

int main(int argc, char **argv) {
    if (argc > 2) {
        N = atoi(argv[1]);
        P = atoi(argv[2]);
    } else {
        printf("What size chessboard do we use?\n");
        fflush(stdout);
        if (scanf("%u", &N) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
        printf("How many processors do we use?\n");
        fflush(stdout);
        if (scanf("%u", &P) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
    }

    bsp_init(&parallelQueens, argc, argv);
    parallelQueens();

    return EXIT_SUCCESS;
}
