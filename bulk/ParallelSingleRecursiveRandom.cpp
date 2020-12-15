#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include "DiagonalBitSet.h"
#include "ParallelSingleRecursiveRandom.h"


#define PRINT_SOLUTION
//#define PRINT_START_VECTOR

// Decide whether to distribute the cases, or have each
// processor start with a random start permutation.
// At least one should be defined.
#define DISTRIBUTE
#define RANDOM

const static uint32_t FOUND = -1;
const static uint32_t SYNC_THRESHOLD = 2000000;

using Board = std::vector<uint32_t>;

uint32_t ParallelSingleRecursiveRandom::findSolution(bulk::world &world, Board &board, uint32_t idx, DiagonalBitSet &diag) {
    if (idx == N) {
        return FOUND;
    }

    uint32_t iterCount = 0;
    for (uint32_t i = idx; i < N; i++) {
        iterCount++;

        if (!diag.hasInterference(idx, board[i])) {
            diag.set(idx, board[i]);
            std::swap(board[idx], board[i]);
            uint32_t res = findSolution(world, board, idx + 1, diag);
            if (res == FOUND) return res;
            iterCount += res;
            std::swap(board[idx], board[i]);
            diag.reset(idx, board[i]);
        }

        if (iterCount > SYNC_THRESHOLD) {
            iterCount = 0;
            world.sync();
        }
    }
    return iterCount;
}


void ParallelSingleRecursiveRandom::solve() {
    bulk::thread::environment env;

#ifdef RANDOM
#ifdef DISTRIBUTE
    uint64_t SEED = time(0) * N * P;
#endif
#endif

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
        srand(time(0) * (s + 1) * p);
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
            srand(SEED);
            for (uint32_t i = 0; i < N; i ++) board[i] = i;
#ifdef RANDOM
            shuffle(board);
#endif
            diag.resetAll();

            // Set board and diagonals
            uint32_t i = caseNr, j = 0, idx;
            for (; j < depth; j++) {
                idx = caseNr % (N - j);
                i /= N - j;
                if (diag.hasInterference(j, board[j + idx])) break;
                diag.set(j, board[j + idx]);
                std::swap(board[j], board[j+idx]);
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

        // Find solution
        findSolution(world, board, start_idx, diag);

#ifdef PRINT_SOLUTION
        // Print found solution
        std::cout << "(";
        for (uint32_t v : board) {
            std::cout << v << ", ";
        }
        std::cout << ") by " << s << " / " << p << "\n";
#endif
        // Abort the other processes
        world.abort();
    });
}
