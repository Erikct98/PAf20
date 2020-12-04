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

        // Compute number of cases
        uint32_t depth = 0, cases = 1;
        while (p * p > cases) {
            cases *= N - depth;
            depth++;
        }

        // Count correct boards in all cases
        Board board(N, 0);

        uint32_t count = 0;
        for (uint32_t caseNr = s; caseNr < cases; caseNr += p) {
            // Initialize board
            for (uint32_t j = 0; j < N; j++) board[j] = j;

            // Initialize diagonal
            DiagonalBitSet diag{N};

            // Setup case
            uint32_t i = caseNr, j, idx;
            for (j = 0; j < depth; j++) {
                idx = i % (N - j);
                i /= (N - j);
                if (diag.hasInterference(j, board[j + idx])) break;
                diag.set(j, board[j + idx]);
                std::swap(board[j], board[j + idx]);
            }
            if (j != depth) continue; // Diagonal clash -> go to next case

            // Count number of solutions
            count += countSolutions(board, depth, diag);
        }

        // Combine all counts
        auto countExchange = bulk::coarray<uint32_t>(world, p);
        for (uint32_t k = 0; k < p; k++) {
            countExchange(k)[s] = count;
        }
        world.sync();
        uint32_t totalCount = 0;
        for (auto c : countExchange) {
            totalCount += c;
        }

        if (s == 0) {
            std::cout << "In total " << totalCount << " solutions were found.\n";
        }
    });
}