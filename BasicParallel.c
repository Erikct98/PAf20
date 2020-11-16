#include "chess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int P;
static int N;

bool diagonalInterference(long *board, int idx){
    long idxHeight = board[idx];
    int horDiff = idx;
    for (int i = 0; i < idx; i++){
        long verDiff = abs(idxHeight - board[i]);
        if (horDiff == verDiff) return true;
        horDiff --;
    }
    return false;
}

bool recursiveFindQueens(long *board, int idx, long *remaining, int len);
bool recursiveFindQueens(long *board, int idx, long *remaining, int len){
    if (idx == N) return true;
    for (int i = 0; i < len; i++){
        board[idx] = remaining[i];
        remaining[i] = remaining[len - 1];
        if (!diagonalInterference(board, idx) && recursiveFindQueens(board, idx + 1, remaining, len - 1)){
            return true;
        }
        remaining[len - 1] = remaining[i];
        remaining[i] = board[idx];
    }
    return false;
}


void parallelQueens(){
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    startTime = bsp_time();

    /** Setup structs **/
    // Local
    long *board = vecalloci(N);
    long *remaining = vecalloci(N);
    
    /** Determine which problem this pid should tackle **/
    if (P <= N) {
        for (long i = pid; i < N; i += P) {
            // Reset remaining
            for (int j = 0; j < N; j ++) remaining[j] = j;

            // Setup case
            board[0] = remaining[i];
            remaining[i] = remaining[N - 1];
            bool found = recursiveFindQueens(board, 1, remaining, N-1);
            if (found) {
                fancyPrintBoard(N, board);
                break;
            }
        }
    } else {
        // P > N
    }
    endTime = bsp_time();

    printf("pid %ld - runtime %f\n", pid, endTime - startTime);

    bsp_end();
}

int main(int argc, char ** argv) {
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
