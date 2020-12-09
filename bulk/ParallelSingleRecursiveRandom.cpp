#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include <iostream>
#include "chess.h"
#include "DiagonalBitSet.h"
#include "ParallelSingleRecursiveRandom.h"


const static uint32_t FOUND = -1;
const static uint32_t STOPPING = -2;
const static uint32_t SYNC_THRESHOLD = 2000000;

using Board = std::vector<uint32_t>;

uint32_t ParallelSingleRecursiveRandom::findSolution(bulk::world &world, Board &board, uint32_t idx, DiagonalBitSet &diag, bulk::var<bool> *done) {
    if (idx == N) {
        auto p = world.active_processors();
        for (int j = 0; j < p; j++) bulk::put((world.rank() + j) % p, true, *done);
        return FOUND;
    }

    uint32_t count = 0;
    for (uint32_t i = idx; i < N; i++) {
        count++;
        if (!diag.hasInterference(idx, board[i])) {
            diag.set(idx, board[i]);
            std::swap(board[idx], board[i]);
            uint32_t res = findSolution(world, board, idx + 1, diag, done);
            if (res == FOUND || res == STOPPING) return res;
            count += res;
          std::swap(board[idx], board[i]);
            diag.reset(idx, board[i]);
        }

        if (count > SYNC_THRESHOLD) {
            count = 0;
            world.sync();
            if (*done) return STOPPING;
        }
    }
    return count;
}


void ParallelSingleRecursiveRandom::solve() {
    bulk::thread::environment env;
    env.spawn(P, [&](bulk::world &world) {
        auto s = world.rank();
        auto p = world.active_processors();

        // Initialize board
        Board board(N, 0);
        for (uint32_t j = 0; j < N; j++) board[j] = j;
        // TODO: shuffle

        // Initialize diagonal
        DiagonalBitSet diag{N};

        // Initialize found flag
        auto done = bulk::var<bool>(world);
        done = false;
        world.sync();

        // Find solution
        auto res = findSolution(world, board, 0, diag, &done);
        if (res == FOUND) {
            std::cout << "(";
            for (uint32_t v : board) {
                std::cout << v << ", ";
            }
            std::cout << ")\n";
            std::cout << "By " << s << " / " << p << '\n';
        }
    });
}
