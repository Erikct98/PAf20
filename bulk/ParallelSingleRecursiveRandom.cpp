#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include <unistd.h>
#include "DiagonalBitSet.h"
#include "ParallelSingleRecursiveRandom.h"


//#define PRINT_SOLUTION
//#define PRINT_START_VECTOR

// Decide whether to distribute the cases, or have each
// processor start with a random start permutation.
// At least one should be defined.
#define DISTRIBUTE
#define RANDOM

// dist
// rand
// dist_rand

using Board = std::vector<uint32_t>;

bool ParallelSingleRecursiveRandom::findSolution(bulk::world &world, Board &board, uint32_t idx,
                                                 DiagonalBitSet &diag, bulk::var<bool>& done, uint32_t& iterCount) {
    for (uint32_t i = idx; i < N; i++) {
        ++iterCount;
        if (!diag.hasInterference(idx, board[i])) {
            diag.set(idx, board[i]);
            std::swap(board[idx], board[i]);
            bool found = findSolution(world, board, idx + 1, diag, done, iterCount);
            if (found) return true;
            std::swap(board[idx], board[i]);
            diag.reset(idx, board[i]);
        }
        if (iterCount > SYNC_THRESHOLD) {
            iterCount = 0;
//            world.log("%d Checking....", world.rank());
            world.sync();
            if (done.value()) {
//                world.log("%d Aborting!", world.rank());
                return true;
            }
        }
    }
    return idx == N;
}


void ParallelSingleRecursiveRandom::solve() {
    bulk::thread::environment env;

    env.spawn(P, [&](bulk::world &world) {
        auto s = world.rank();
        auto p = world.active_processors();

        // Initialize board
        Board board(N, 0);

        // Set start index
        uint32_t start_idx = 0;

        // Initialize diagonal
        DiagonalBitSet diag{N};

#ifdef RANDOM
        // Each processor chooses an arbitrary starting permutation
        // and starts searching
        {
            uint32_t a = SEED;
            uint32_t b = s + 1;
            uint32_t c = p;

            a=a-b;  a=a-c;  a=a^(c >> 13u);
            b=b-c;  b=b-a;  b=b^(a << 8u);
            c=c-a;  c=c-b;  c=c^(b >> 13u);
            a=a-b;  a=a-c;  a=a^(c >> 12u);
            b=b-c;  b=b-a;  b=b^(a << 16u);
            c=c-a;  c=c-b;  c=c^(b >> 5u);
            a=a-b;  a=a-c;  a=a^(c >> 3u);
            b=b-c;  b=b-a;  b=b^(a << 10u);
            c=c-a;  c=c-b;  c=c^(b >> 15u);

            srand(c);
        }

#ifndef DISTRIBUTE
        for (uint32_t j = 0; j < N; j++) board[j] = j;
        shuffle(board);
        start_idx = 0;
#endif
#endif

#ifdef DISTRIBUTE
        // We start with the identity permutation and distribute the cases
        // among the processes

        // Determine the number of cases
        uint32_t depth = 0, nrCases = 1;
        while (10 * p > nrCases) {
            nrCases *= N - depth;
            ++depth;
        }

        // Determine local case
        bool found = false;
        for (uint32_t caseNr = s; caseNr < nrCases && !found; caseNr += p) {
            // Reset seed, board and diagonals

            for (uint32_t i = 0; i < N; i ++) board[i] = i;
#ifdef RANDOM
            srand(SEED);
            shuffle(board);
#endif
            diag.resetAll();

            // Set board and diagonals
            uint32_t i = caseNr, j = 0, idx;
            for (; j < depth; j++) {
                idx = i % (N - j);
                i /= N - j;
                if (diag.hasInterference(j, board[j + idx])) break;
                diag.set(j, board[j + idx]);
                std::swap(board[j], board[j + idx]);
            }
            found = j == depth;
        }

        start_idx = depth;
#endif

#ifdef PRINT_START_VECTOR
        // Print start vector
        std::cout << "(";
        for (uint32_t i = 0; i < start_idx; i++) {
            std::cout << board[i] << ", ";
        }
        std::cout << ") by " << s << " / " << p << "\n";
#endif
        auto done = bulk::var<bool>(world);
        uint32_t iterCount = 0;

        // Find solution
        findSolution(world, board, start_idx, diag, done, iterCount);

        // Stop the other processes
        if (!done.value()) {
            done = true;
//            world.log("%d Found something!", world.rank());
            for (int i = 0; i < p; i++) {
                bulk::put(i, true, done);
            }
            world.sync();
#ifdef PRINT_SOLUTION
            // Print found solution
            std::cout << "(";
            for (uint32_t v : board) {
                std::cout << v << ", ";
            }
            std::cout << ") by " << s << " / " << p << "\n";
#endif
        } else {
#ifdef PRINT_SOLUTION
            std::cout << s << " found nothing!\n";
#endif
        }
    });
}

ParallelSingleRecursiveRandom::ParallelSingleRecursiveRandom(int N, int P) : N(N), P(P) {
    uint32_t a = clock();
    uint32_t b = time(nullptr);
    uint32_t c = getpid();

    a=a-b;  a=a-c;  a=a^(c >> 13u);
    b=b-c;  b=b-a;  b=b^(a << 8u);
    c=c-a;  c=c-b;  c=c^(b >> 13u);
    a=a-b;  a=a-c;  a=a^(c >> 12u);
    b=b-c;  b=b-a;  b=b^(a << 16u);
    c=c-a;  c=c-b;  c=c^(b >> 5u);
    a=a-b;  a=a-c;  a=a^(c >> 3u);
    b=b-c;  b=b-a;  b=b^(a << 10u);
    c=c-a;  c=c-b;  c=c^(b >> 15u);

    SEED = c;
}
