#ifndef PAF20_CHESS_H
#define PAF20_CHESS_H

#include <stdio.h>
#include <math.h>
#include "edupack/bspedupack.h"

int *vecallocint(size_t n){
    /* This function allocates a vector of long
       integers of length n */
    int *pi;

    pi = malloc(MAX(n,1)*sizeof(int));
    if (pi == NULL) {
        bsp_abort("vecalloci: not enough memory");
    }

    return pi;
}

void vecfreeint(int *pi){
    /* This function frees a vector of integers */

    if (pi!=NULL)
        free(pi);

} /* end vecfreeint */

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. FROM: https://stackoverflow.com/questions/6127503/shuffle-array-in-c */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void seedRandom() {
    unsigned long a = clock();
    unsigned long b = time(NULL);
    unsigned long c = getpid();

    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);

    srand(c);
}

void printGrid(int N, char* chars) {
    for (int i = 0; i < N; i++) {
        printf("%2x ", N - 1 - i);
        for (int j = 0; j < N; ++j) {
            printf("%c", chars[i * N + j]);
        }
        printf("\n");
    }
    printf(" - ");
    for (int i = 0; i < N; ++i) {
        printf("%x", i % 16);
    }
    printf("\n");
}

void printFancyGrid(int N, char* chars) {
    for (int i = 0; i < N; ++i) {
        // Horizontal divider
        for (int j = 0; j < N+1; j++){
            printf(" - +");
        }
        printf("\n");

        // Line content
        printf("%2x |", N - 1 - i);
        for (int j = 0; j < N; ++j) {
            printf(" %c |", chars[i * N + j]);
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

const char bad_queen_c = '@';
const char queen_c = '#';
const char no_queen_c = '?';
const char nothing_c = ' ';

char* boardToChar(int N, int* board) {
    char* chars = malloc((N * N + 1) * sizeof(char));
    if (!chars) {
        printf("Malloc failed for creating chars!\n");
        exit(-1);
    }
    memset(chars, nothing_c, N * N * sizeof(char));
    chars[N * N] = '\0';
    for (int j = 0; j < N; ++j) {
        chars[(N - 1 - board[j]) * N + j] = queen_c;
    }

    return chars;
}

void printBoard(int N, int* board) {
    char* str = boardToChar(N, board);
    printGrid(N, str);
    free(str);
}

void fancyPrintBoard(int N, int* board) {
    char* str = boardToChar(N, board);
    printFancyGrid(N, str);
    free(str);
}

bool validBoard(int N, int* board, bool printError) {
    // it does not matter whether we treat the board as columns or rows
    int* rowValue = malloc(N * sizeof(int));
    memset(rowValue, -1, N * sizeof(int));

    bool failed = false;

    char* error = malloc((N * N + 1) * sizeof(char));
    memset(error, nothing_c, N * N * sizeof(char));
    error[N * N] = '\0';

    for (int i = 0; i < N; i++) {
        if (board[i] < 0 || board[i] >= N) {
            printf("Column %d does not have a placed queen (invalid value / missing?)\n", i);
            failed = true;
            for (int j = 0; j < N; ++j) {
                error[(N - 1 - j) * N + i] = no_queen_c;
            }
        } else {
            error[(N - 1 - board[i]) * N + i] = queen_c;
            if (rowValue[board[i]] >= 0) {
                int other = rowValue[board[i]];
                error[(N - 1 - board[i]) * N + i] = bad_queen_c;
                error[(N - 1 - board[i]) * N + other] = bad_queen_c;
                for (int k = fmin(i, other) + 1; k < fmax(i, other); ++k) {
                    int index = (N - 1 - board[i]) * N + k;
                    if (error[index] == nothing_c || error[index] == no_queen_c) {
                        error[index] = '-';
                    }
                }
                printf("Row %d has at least two queens: %d and %d\n", board[i], rowValue[board[i]], i);
                failed = true;
            }
            rowValue[board[i]] = i;
        }
    }

    // dont think this can actually happen without failed but just to be complete
    for (int i = 0; i < N; ++i) {
        if (rowValue[i] < 0) {
            printf("Row %d has no queen\n", i);
            for (int j = 0; j < N; ++j) {
                error[(N - 1 - i) * N + j] = no_queen_c;
            }
            failed = true;
        }
    }
#ifdef EARLY_EXIT_VALIDATE
    if (failed) {
        printFancyGrid(N, error);

        free(rowValue);
        free(error);
        return false;
    }
#endif

    for (int i = 0; i < N; i++) {
        int base = board[i];
        for (int j = 1; i + j < N; j++) {
            if (board[i + j] == base + j) {
                // right and up
                printf("Two queens occupying the same diagonal: at (%d, %d) and (%d, %d)\n", i, board[i], i + j, board[i + j]);
                failed = true;
                error[(N - 1 - board[i]) * N + i] = bad_queen_c;
                error[(N - 1 - board[i + j]) * N + (i + j)] = bad_queen_c;
                for (int k = 1; k < j; ++k) {
                    int index = (N - 1 - base - k) * N + (i + k);
                    if (error[index] == nothing_c || error[index] == no_queen_c) {
                        error[index] = '/';
                    } else if (error[index] == '\\') {
                        error[index] = 'X';
                    }
                }
            }
            if (board[i + j] == base - j) {
                // right and down
                printf("Two queens occupying the same diagonal: at (%d, %d) and (%d, %d)\n", i, board[i], i + j, board[i + j]);
                failed = true;
                error[(N - 1 - board[i]) * N + i] = bad_queen_c;
                error[(N - 1 - board[i + j]) * N + (i + j)] = bad_queen_c;
                for (int k = 1; k < j; ++k) {
                    int index = (N - 1 - base + k) * N + (i + k);
                    if (error[index] == nothing_c || error[index] == no_queen_c) {
                        error[index] = '\\';
                    } else if (error[index] == '/') {
                        error[index] = 'X';
                    }
                }
            }
        }
    }

    if (printError && failed) {
        printGrid(N, error);
    }

    free(error);
    free(rowValue);
    return failed;
}

#endif //PAF20_CHESS_H
