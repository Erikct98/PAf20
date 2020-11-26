#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int N = 0;
long long unsigned iter = 0;

bool hasInterference(int* board, int placed, int next) {
    if (placed < 0) {
        return false;
    }
    int horDiff = placed + 1;
    for (int i = 0; i <= placed; i++) {
        int verDiff = next - board[i];
        if (horDiff == verDiff || horDiff == -verDiff) {
            return true;
        }
        horDiff--;
    }
    return false;
}

bool interference(int *board, int idx) {
    return hasInterference(board, idx - 1, board[idx]);
}

bool recursiveCall(int *board, int idx, int *remaining, int len);

bool recursiveCall(int *board, int idx, int *remaining, int len) {
//    iter++;
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

void printList(int* val, int len) {
    for(int loop = 0; loop < len; loop++)
        printf("%d ", val[loop]);

    printf("\n");
}

bool solveNonRecursive(int* board, int idx, int* remaining, int len) {
    // given board which we fill in from idx...N-1

    int* remainingPicks = malloc(len * sizeof(int));
    memset(remainingPicks, -1, len * sizeof(int));

    bool* diagUp = malloc((2 * N - 1) * sizeof(bool));
    bool* diagDown = malloc((2 * N - 1) * sizeof(bool));

    memset(diagUp, false, (2 * N - 1) * sizeof(bool));
    memset(diagDown, false, (2 * N - 1) * sizeof(bool));

#define DIAG_UP(x, val) diagUp[(val - x) + (N - 1)]
#define DIAG_DOWN(x, val) diagDown[x + val]
#define SET_DIAG(x, val, set) \
    DIAG_UP(x, val) = set;    \
    DIAG_DOWN(x, val) = set

    for (int i = 0; i < idx; ++i) {
        DIAG_UP(i, board[i]) = true;
        DIAG_DOWN(i, board[i]) = true;
    }

    int currIdx = idx;
    int remainingBack = len;
    int remainingIdx = 0;
    while (currIdx < N) {
        while (remainingIdx < remainingBack && (DIAG_UP(currIdx, remaining[remainingIdx]) || DIAG_DOWN(currIdx, remaining[remainingIdx]))) {
            ++remainingIdx;
        }
        if (remainingIdx >= remainingBack) {
//            printf("Cannot place with previous choices!\n");
            currIdx--;
            if (currIdx < idx) {
                break;
            }
            remainingIdx = remainingPicks[currIdx - idx];
            remainingPicks[currIdx - idx] = -1;
            remaining[remainingIdx] = board[currIdx];
            SET_DIAG(currIdx, board[currIdx], false);
            board[currIdx] = -1;
            remainingIdx++;
            remainingBack++;
            continue;
        } else {
            board[currIdx] = remaining[remainingIdx];
            SET_DIAG(currIdx, board[currIdx], true);
            remainingPicks[currIdx - idx] = remainingIdx;
            remaining[remainingIdx] = remaining[remainingBack - 1];
            --remainingBack;
            remainingIdx = 0;
            ++currIdx;
        }
    }

    free(diagUp);
    free(diagDown);
    free(remainingPicks);

    return currIdx == N;
}

void recursiveSolve(bool recursive) {
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

    shuffle(remaining, N);

    // Start recursive call
    bool foundSolution;
    if (recursive) {
        foundSolution = recursiveCall(board, 0, remaining, N);
    } else {
        foundSolution = solveNonRecursive(board, 0, remaining, N);
    }

    // End timing
    endTime = clock();

    // Print result
    if (!foundSolution) {
        printf("Failed to find a solution!\n");
    } else {
        if (N < 64) {
            printBoard(N, board);
        }
        if (validBoard(N, board, false)) {
            foundSolution = false;
            printf("Board is valid!\n");
        }
    }


    free(board);
    free(remaining);

    // Report running time
    clock_t time_taken = (endTime - startTime);
    printf("[valid=%s] Solving %d-queens took %s: %f\n",
           foundSolution ? "true" : "false",
           N, recursive ? "recursive seq" : "iter seq",
           (double) time_taken / CLOCKS_PER_SEC);
}


int main(int argc, char ** argv) {
    if (argc > 1) {
        printf("%s\n", argv[1]);
        N = strtoul(argv[1], NULL, 10);
    } else {
        printf("What size chessboard do we use?\n");
        fflush(stdout);
        if (scanf("%ul", &N) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
    }

    long seed = seedRandom();
    recursiveSolve(true);

    srand(seed);

    recursiveSolve(false);

    return EXIT_SUCCESS;
}
