#include "CountRecursive.h"
#include <bulk/backends/thread/environment.hpp>
#include <chrono>

CountRecursive::CountRecursive(uint32_t n, uint32_t procs) : N(n), procs(procs) {}

using Board = std::vector<uint32_t>;


uint64_t CountRecursive::countSolutions(std::vector<uint32_t> &board, uint32_t idx, DiagonalBitSet &diag) {
    if (idx == board.size()) {
        // should never happen!
        return 1;
    }
    if (idx == board.size() - 1) {
        return !diag.hasInterference(idx, board[idx]);
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

//#define REFLECT_SIMPLE
//#define TIME_INDIVIDUAL

#ifdef TIME_INDIVIDUAL
#define WHEN_TIMING(x) x
#else
#define WHEN_TIMING(x)
#endif

uint64_t CountRecursive::solve() {
    WHEN_TIMING(auto begin = std::chrono::steady_clock::now();)
    bulk::thread::environment env;
    uint64_t val;
    env.spawn(procs, [&](bulk::world &world) {
        auto s = world.rank();
        auto p = world.active_processors();

        // Compute number of cases
        uint32_t depth = 1;
#ifdef REFLECT_SIMPLE
        uint32_t cases = (N + 1) / 2;
#else
        uint32_t cases = N;
#endif
        while (p * p > cases && depth < N) {
            cases *= N - depth;
            depth++;
        }

//        std::cout << "Cases: " << cases << '\n';

        // Count correct boards in all cases
        Board board(N, 0);

        WHEN_TIMING(uint32_t realCase = 0;)

        // Initialize diagonal
        DiagonalBitSet diag{N};
#ifdef REFLECT_SIMPLE
        bool isOdd = N % 2 == 1;
        uint32_t halfWay = (N + 1) / 2;
#endif

        auto count = bulk::var<uint64_t>(world, 0u);
        for (uint32_t caseNr = s; caseNr < cases; caseNr += p) {
            // Initialize board
            for (uint32_t j = 0; j < N; j++) board[j] = j;

            diag.resetAll();

            // Setup case
            uint32_t i = caseNr, j, idx;
            for (j = 0; j < depth; j++) {
#ifdef REFLECT_SIMPLE
                if (j == 0) {
                    idx = i % halfWay;
                    i /= halfWay;
                } else {
#endif
                    idx = i % (N - j);
                    i /= (N - j);
#ifdef REFLECT_SIMPLE
                }
#endif
                if (diag.hasInterference(j, board[j + idx])) break;
                diag.set(j, board[j + idx]);
                std::swap(board[j], board[j + idx]);
            }
            if (j != depth) {
                continue;// Diagonal clash -> go to next case
            }

            uint64_t solutions = countSolutions(board, depth, diag);
#ifdef REFLECT_SIMPLE
            if (isOdd && caseNr % halfWay == halfWay - 1) {
#endif
                count += solutions;
#ifdef REFLECT_SIMPLE
            } else {
                count += 2 * solutions;
            }
#endif
            WHEN_TIMING(realCase++;)
        }

        WHEN_TIMING(
            auto end = std::chrono::steady_clock::now();
            auto duration = end - begin;
            world.log("%ud had %ud real cases count %lud... %f (in %ll ms, %d s)", s, realCase, count.value(), static_cast<float>(cases) / p, std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count(), std::chrono::duration_cast<std::chrono::seconds>(duration).count());
        )

        // Combine all counts
        uint64_t totalCount = bulk::sum(count);

        if (s == 0) {
            val = totalCount;
        }
    });

    return val;
}