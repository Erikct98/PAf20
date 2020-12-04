#include "CountRecursive.h"
#include <bulk/backends/thread/environment.hpp>

CountRecursive::CountRecursive(uint32_t n, uint32_t procs) : N(n), procs(procs) {}


uint32_t CountRecursive::countSolutions(Board &board, uint32_t idx, DiagonalBitSet &diag) {
    if (idx == board.size()) {
        return 1;
    }

    uint32_t count = 0;
    for (uint32_t i = idx; i < board.size(); i++) {
        if (!diag.hasInterference(idx, board[i])) {
            diag.set(idx, board[i]);
            std::swap(board[idx], board[i]);
            count += countSolutions(board, idx + 1, diag);
            std::swap(board[i], board[idx]);
            diag.reset(idx, board[i]);
        }
    }
    return count;
}



void CountRecursive::solve() {
    bulk::thread::environment env;
    env.spawn(procs, [&](bulk::world &world) {
        auto s = world.rank();
        auto p = world.active_processors();

        // Initialize board
        Board board(N, 0);
        for (uint32_t j = 0; j < N; j++) board[j] = j;

        // Initialize diagonal
        DiagonalBitSet diag{N};

        // Find solution
        auto res = countSolutions(board, 0, diag);
        std::cout << "Proc " << s << " Found " << res << " solutions!\n";
    });
}