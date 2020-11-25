#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int P;
static int N;

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

bool recursiveFindQueens(int *board, int idx, int *fwdDiag, int *bwdDiag);

bool recursiveFindQueens(int *board, int idx, int *fwdDiag, int *bwdDiag) {
    if (idx == N) return true;

    int fwdDiagIdx, bwdDiagIdx;
    for (int i = idx; i < N; i++) {
        swap(&board[idx], &board[i]);
        fwdDiagIdx = getFwdDiagIdx(board, idx);
        bwdDiagIdx = getBwdDiagIdx(board, idx);
        if (!fwdDiag[fwdDiagIdx] && !bwdDiag[bwdDiagIdx]) {
            fwdDiag[fwdDiagIdx] = true;
            bwdDiag[bwdDiagIdx] = true;
            if (recursiveFindQueens(board, idx + 1, fwdDiag, bwdDiag)) {
                return true;
            }
            fwdDiag[fwdDiagIdx] = false;
            bwdDiag[bwdDiagIdx] = false;
        }
        swap(&board[idx], &board[i]);
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
    int *board = vecallocint(N);
    int *fwdDiag = vecallocint(2 * N - 1);
    int *bwdDiag = vecallocint(2 * N - 1);

    /** Determine iteration depth **/
    long depth = 0, cases = 1;
    while (P > cases) {
        cases *= N - depth;
        depth++;
    }

    /** Tackle assigned problems **/
    for (long caseNr = pid; caseNr < cases; caseNr += P) {
        // Set start values
        for (int j = 0; j < N; j++) board[j] = j; // TODO: randomization -> use a random permutation
        memset(fwdDiag, false, 2 * N - 1);
        memset(bwdDiag, false, 2 * N - 1);

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
        bool found = recursiveFindQueens(board, depth, fwdDiag, bwdDiag);
        if (found && validBoard(N, board, true)) {
            fancyPrintBoard(N, board);
            break;
        }
    }

    endTime = bsp_time();

    printf("pid %ld - runtime %f\n", pid, endTime - startTime);

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
    parallelQueens();

    return EXIT_SUCCESS;
}
