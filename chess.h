#ifndef PAF20_CHESS_H
#define PAF20_CHESS_H

#include <stdio.h>
#include "edupack/bspedupack.h"


void printBoard(int N, int* board) {
    for (int i = N - 1; i >= 0; --i) {
        printf("%2x ", i);
        for (int j = 0; j < N; ++j) {
            if (board[j] == i) {
//                printf("♕");
                printf("#");
            } else {
                printf(" ");
            }
//            printf(" %*s#\n", i + 1, board[i], "");
        }
        printf("\n");
    }
    printf(" - ");
    for (int i = 0; i < N; ++i) {
        printf("%x", i % 16);
    }
    printf("\n");
}

void fancyPrintBoard(int N, int* board) {
    for (int i = N - 1; i >= 0; --i) {
        // Horizontal divider
        for (int j = 0; j < N+1; j++){
            printf(" - +");
        }
        printf("\n");

        // Line content
        printf("%2x |", i);
        for (int j = 0; j < N; ++j) {
            printf(board[j] == i ? " # |" : "   |");
        }
        printf("\n");
    }
    // Final horizontal divider
    for (int j = 0; j < N+1; j++){
        printf(" - +");
    }
    printf("\n");

    // Column numbers
    printf("     ");
    for (int i = 0; i < N; ++i) {
        printf("%x   ", i % 16);
    }
    printf("\n");
}


#endif //PAF20_CHESS_H
