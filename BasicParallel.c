#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int P;
static int N;
static int CHECK_THRESHOLD = 20000000;
static unsigned long SEED;

static int FOUND = -1;
static int STOPPING = -2;

void swap(int *one, int *two) {
    int temp = *one;
    *one = *two;
    *two = temp;
}

int getFwdDiagIdx(int *board, int idx) {
    return board[idx] + N - 1 - idx;
}

int getBwdDiagIdx(int *board, int idx) {
    return board[idx] + idx;
}

int recursiveFindQueens(int *board, int idx, int *fwdDiag, int *bwdDiag, bool *done);

int recursiveFindQueens(int *board, int idx, int *fwdDiag, int *bwdDiag, bool *done) {
    if (idx == N) {
        // Set done flags
        bool ans = true;
        for (int i = 0; i < P; i++) bsp_put(i, &ans, done, 0, sizeof(bool));
        return FOUND;
    }

    int fwdDiagIdx, bwdDiagIdx, count = 0;
    for (int i = idx; i < N; i++) {
        swap(&board[idx], &board[i]);
        fwdDiagIdx = getFwdDiagIdx(board, idx);
        bwdDiagIdx = getBwdDiagIdx(board, idx);
        if (!fwdDiag[fwdDiagIdx] && !bwdDiag[bwdDiagIdx]) {
            fwdDiag[fwdDiagIdx] = true;
            bwdDiag[bwdDiagIdx] = true;
            int val = recursiveFindQueens(board, idx + 1, fwdDiag, bwdDiag, done);
            if (val < 0) return val;
            count += val;
            if (count > CHECK_THRESHOLD) {
                bsp_sync();
                if (*done) return STOPPING;
                count = 0; // Reset count; we are synchronizing
            }
            fwdDiag[fwdDiagIdx] = false;
            bwdDiag[bwdDiagIdx] = false;
            count ++;
        }
        swap(&board[idx], &board[i]);
    }
    return count;
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
    int *board = vecallocint(N);
    int *fwdDiag = vecallocint(2 * N - 1);
    int *bwdDiag = vecallocint(2 * N - 1);

    // Shared
    bool done = false;
    bsp_push_reg(&done, sizeof(bool));
    bsp_sync();

    /** Determine iteration depth **/
    long depth = 0, cases = 1;
    while (P > cases) {
        cases *= N - depth;
        depth++;
    }

    /** Tackle assigned problems **/
    for (long caseNr = pid; caseNr < cases; caseNr += P) {
        // Set start values
        for (int j = 0; j < N; j++) board[j] = j;
        srand(SEED);
        shuffle(board, N);
        memset(fwdDiag, false, (2 * N - 1) * sizeof(bool));
        memset(bwdDiag, false, (2 * N - 1) * sizeof(bool));

        // Setup starting point for search
        int i = caseNr, j, idx;
        int fwdDiagIdx, bwdDiagIdx;
        for (j = 0; j < depth; j++) {
            idx = i % (N - j);
            i /= (N - j);
            swap(&board[j], &board[j + idx]);
            fwdDiagIdx = getFwdDiagIdx(board, j);
            bwdDiagIdx = getBwdDiagIdx(board, j);
            if (!fwdDiag[fwdDiagIdx] && !bwdDiag[bwdDiagIdx]) {
                fwdDiag[fwdDiagIdx] = true;
                bwdDiag[bwdDiagIdx] = true;
            } else break;
        }
        if (j != depth) continue; // Next case if diagonal clash

        // Search for solution
        int val = recursiveFindQueens(board, depth, fwdDiag, bwdDiag, &done);
        if (val == FOUND && validBoard(N, board, true)) {
            printBoard(N, board);
            bsp_sync();
        }
        if (val < 0) break; // Either FOUND or STOPPING
    }

    endTime = bsp_time();

    printf("pid %ld - runtime %f\n", pid, endTime - startTime);

    bsp_pop_reg(&done);
    vecfreeint(board);
    vecfreeint(fwdDiag);
    vecfreeint(bwdDiag);

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
    SEED = seedRandom();
    parallelQueens();

    return EXIT_SUCCESS;
}
