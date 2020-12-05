#include "CountIterating.h"
#include <bulk/backends/thread/environment.hpp>

CountIterating::CountIterating(uint32_t n, uint32_t procs) : N(n), procs(procs) {}

uint32_t CountIterating::countSolutions(Board &board) const {
    uint32_t count = 0;

    while (board.getIdx() < 0xffffffff) {
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
            cases *= N;
            depth++;
        }

        // Init board
        Board board(N);
        uint32_t count = countSolutions(board);
        std::cout << "count: " << count << std::endl;
    });
}


