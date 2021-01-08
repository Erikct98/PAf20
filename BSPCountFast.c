#include "edupack/bspedupack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define DETAIL_PRINT

#ifdef DETAIL_PRINT
#define output(...) printf(__VA_ARGS__)
#else
#define output(...)
#endif

typedef long long unsigned ll;

ll answers[] = {
        1,
        1,
        0,
        0,
        2,
        10,
        4,
        40,
        92,
        352,
        724,
        2680,
        14200,
        73712,
        365596,
        2279184,
        14772512,
        95815104,
        666090624,
        4968057848,
        39029188884,
        314666222712,
        2691008701644,
        24233937684440,
        227514171973736,
        2207893435808352,
        22317699616364044,
        234907967154122528,
};


static int P;
static int N;
static ll ANSWER = 0;

bool bit_set(ll val, int idx) {
    return (val & (1u << idx)) != 0;
}

struct Board {

    ll finalMask;
    int rem;

    ll* places;
    ll* tried;

    ll rows;
    ll diagUp;
    ll diagDown;
    int idx;
};

void reset_board(struct Board* board) {
    board->rows = 0u;
    board->diagUp = 0u;
    board->diagDown = 0u;
    board->idx = 0;
}

struct Board create_board(ll size) {
    struct Board board;
    board.rem = 64u - size;
    board.finalMask = (1u << size) - 1;
    board.places = malloc(size * sizeof(ll));
    board.tried = malloc((size + 1u) * sizeof(ll));
    board.tried[0] = board.finalMask;

    return board;
}


ll avail_board(struct Board* board) {
    return (~(board->diagUp | (board->diagDown >> board->rem) | board->rows)) & board->finalMask;
}

void push_board(struct Board* board, ll idx) {
    output("  Pushing: %llu [%d]\n", idx, board->idx);
    board->rows |= idx;
    board->diagUp |= idx;
    board->diagDown |= idx << board->rem;

    board->diagUp <<= 1u;
    board->diagDown >>= 1u;

    board->places[board->idx] = idx;
    board->tried[board->idx + 1] = avail_board(board);
    board->idx++;
}

void pop_board(struct Board* board) {
    board->idx--;
    ll last_queen = board->places[board->idx];
    output("  Popping: %llu [%d]\n", last_queen, board->idx);
    board->diagUp >>= 1u;
    board->diagDown <<= 1u;
    board->rows &= ~last_queen;
    board->diagUp &= ~last_queen;
    board->diagDown &= ~(last_queen << board->rem);
}

void printbits(ll x) {
#ifdef DETAIL_PRINT
    for(int i=6; i; i--)
        putchar('0'+((x>>(i-1))&1));
#endif
}

ll next_spot_board(struct Board* board) {
    bool p = board->tried[board->idx] != 0;
    if (p) {
        output("Get avail from: ");
        printbits(board->tried[board->idx]);
    } else {
        output("Nothing to place\n");
    }
    ll elt = board->tried[board->idx] & -board->tried[board->idx];
    board->tried[board->idx] &= ~elt & board->finalMask;
    if (p) {
        output(" after -> ");
        printbits(board->tried[board->idx]);
        output("\n");
    }
    return elt;
}

void destroy_board(struct Board* board) {
    free(board->places);
    free(board->tried);
}


ll count_board(struct Board* pBoard) {
    ll count = 0u;
    int stopIdx = pBoard->idx - 1;
    output("Stopping at: %d\n", stopIdx);
    int doneIdx = N;

    while (stopIdx != pBoard->idx) {
        if (pBoard->idx == doneIdx) {
            count++;
            output("Got one!\n");
            for (int i = 1; i < pBoard->idx; i++) {
                output("Mask at: %d : ", i);
                printbits(pBoard->tried[i]);
                output("spot: [%llu]\n", pBoard->places[i]);
            }
//            output("Popping at: %llu [%d]\n", pBoard->places[pBoard->idx - 1], pBoard->idx - 1);
            pop_board(pBoard);
        } else {
            ll next = next_spot_board(pBoard);
            if (next == 0) {
//                output("Popping at: %llu [%d]\n", pBoard->places[pBoard->idx - 1], pBoard->idx - 1);
                pop_board(pBoard);
            } else {
                push_board(pBoard, next);
            }
        }
    }

    return count;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void countQueens() {
    double startTime, endTime;

    // Start process
    bsp_begin(P);
    long pid = bsp_pid();

    // Begin timing
    startTime = bsp_time();

    /** Setup structs **/
    // Local
    long *counts = vecalloci(P);
    bsp_push_reg(counts, P * sizeof(long));

    bsp_sync();
    ll count = 0u;

    int depth = 1;
    ll cases = (N + 1) / 2;

    while (P * P > cases && depth < N) {
        cases *= N - depth;
        depth++;
    }

    output("Have depth: %d with %llu cases\n", depth, cases);

    struct Board board = create_board(N);
    bool isOdd = N % 2 == 1;
    int halfWay = (N + 1) / 2;
    int* spots = malloc(N * sizeof(int));



    for (ll caseNr = pid; caseNr < cases; caseNr += P) {
        reset_board(&board);

        for (int i = 0; i < N; i++) {
            spots[i] = i;
        }

        output("Starting search!\n");

        ll currVal = caseNr;
        int j = 1;
        int idx = 0;
        // depth is at least 1 so always run the inital case
        // board is empty -> must be avail and no overlap
        {
            idx = currVal % halfWay;
            currVal /= halfWay;
            ll val = 1u << spots[idx];
            push_board(&board, val);
            swap(&spots[0], &spots[idx]);
        }

        for (; j < depth; j++) {
            idx = currVal % (N - j) + j;
            currVal /= (N - j);

            ll avail = avail_board(&board);
            ll val = 1u << spots[idx];
            if ((avail & val) == 0) {
                break;
            }
            push_board(&board, val);
            swap(&spots[j], &spots[idx]);
        }

        if (j != depth
            || (isOdd && depth > 1 && spots[0] == halfWay - 1 && spots[1] < halfWay)) {
            continue;
        }

        output("placed: ");
        printbits(board.rows);
        output("\n");


        ll solutions = count_board(&board);

#ifdef DETAIL_PRINT
        printf("Given: ");
        for (int i = 0; i <depth; i++) {
            printf("%d, ", spots[i]);
        }
        printf(" got %llu solutions \n", solutions);
#endif

        if (depth == 1 && isOdd && spots[0] == halfWay - 1 && spots[1] < halfWay) {
            count += solutions;
        } else {
            count += 2 * solutions;
        }
    }

#ifdef TIME_PRINT
    endTime = bsp_time();
    printf("pid %ld - runtime %f\n", pid, endTime - startTime);
#endif

    for (int i = 0; i < P; i++) {
        bsp_put(i, &count, counts, pid * sizeof(ll), sizeof(ll));
    }

    bsp_sync();
//    printf("pid %ld got: %llu", pid, count);
    ll total = 0;
    if (pid == 0) {
        endTime = bsp_time();
        for (int i = 0; i < P; i++) {
            output("Counts received: %llu from %d\n", counts[i], i);
            total += counts[i];
        }
        output("Total counts received: %llu\n", total);

        printf("For N=%d got %llu in %f\n", N, total, endTime - startTime);
        ANSWER = total;
    }

    free(spots);
    destroy_board(&board);

    bsp_pop_reg(&counts);
    free(counts);

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

    printf("Sizes: long=%lu, int=%lu, ll=%lu", sizeof(long), sizeof(int), sizeof(ll));

    printf("Running N=%d, P=%d\n", N, P);
    bsp_init(&countQueens, argc, argv);
    countQueens();

    if (N <= 27) {
        if (answers[N] != ANSWER) {
            printf("Wrong answer! got %llu vs. %llu\n", ANSWER, answers[N]);
        } else {
            printf("Got correct answer! for (N=%u)\n", N);
        }
    }

    return EXIT_SUCCESS;
}
