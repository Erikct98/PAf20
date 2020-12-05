#include "CountIterating.h"
#include <bulk/backends/thread/environment.hpp>

CountIterating::CountIterating(uint32_t n, uint32_t procs) : N(n), procs(procs) {}

uint32_t CountIterating::countSolutions(Board &board) const {
    uint32_t count = 0;
    uint64_t endIdx = board.getIdx() - 1; // Dirty hack, but it works

    while (board.getIdx() != endIdx) {
        if (board.getIdx() == N) {
            count += 1;
            board.pop();
        } else {
            uint64_t elt = board.getNextElt();
            if (elt == 0) {
                board.pop();
            } else {
                board.push(elt);
            }
        }
    }

    return count;
}

void CountIterating::solve() {
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

        // Init board
        Board board(N);

        // Count all solutions
        uint32_t count = 0;
        for (uint32_t caseNr = s; caseNr < cases; caseNr += p) {
            // Reset board
            board.reset();

            // Create placeholder
            std::vector<uint64_t> placeHolder(N);
            for (uint32_t i = 0; i < N; i++) placeHolder[i] = i;

            // Setup case
            uint32_t i = caseNr, j, idx;
            for (j = 0; j < depth; j++) {
                idx = i % (N - j);
                i /= (N - j);

                uint64_t available = board.getAvailable();
                if ((available & (1u << placeHolder[j + idx])) == 0) break;
                board.push(1u << placeHolder[j + idx]);
                std::swap(placeHolder[j], placeHolder[j + idx]);
            }
            if (j != depth) continue; // Diagonal clash -> go to next case

            // Count number of solutions
            count += countSolutions(board);
        }

        // Exchange counts
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
            std::cout << "Total count: " << totalCount << std::endl;
        }
    });
}


