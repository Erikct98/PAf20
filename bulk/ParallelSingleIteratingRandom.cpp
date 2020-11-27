#include "ParallelSingleIteratingRandom.h"

class ParallelSingleIteratingRandom {
private:
    ParallelSingleIteratingRandom::findSolution(const Board& partial, RemainingList remaining) {
        DiagonalBitSet diag {N};
        const uint32_t upto = partial.size();

        Board board (N, initValue);

        if (N - upto != remaining.size()) {
            std::cout << "Failure!\n";
            return {};
        }

        for (uint32_t i = 0; i < upto; ++i) {
            board[i] = partial[i];
            diag.set(i, partial[i]);
        }

        std::vector<uint32_t> picks;
        picks.reserve(remaining.size());

        uint32_t currIdx = upto;
        uint32_t remainingIdx = 0;
        auto remainingBack = remaining.size();
        while (currIdx < N) {
            while (remainingIdx < remainingBack && diag.hasInterference(currIdx, remaining[remainingIdx])) {
                ++remainingIdx;
            }
            if (remainingIdx >= remainingBack) {
                currIdx--;
                if (currIdx < upto) {
                    return {};
                }
                remainingIdx = picks.back();
                picks.pop_back();
                remaining[remainingIdx] = board[currIdx];
                diag.reset(currIdx, board[currIdx]);
                remainingIdx++;
                remainingBack++;
            } else {
                board[currIdx] = remaining[remainingIdx];
                diag.set(currIdx, board[currIdx]);
                picks.push_back(remainingIdx);
                remaining[remainingIdx] = remaining[remainingBack - 1];
                --remainingBack;
                remainingIdx = 0;
                ++currIdx;
            }
        }

        return board;
    }

public:
    ParallelSingleIteratingRandom::solve() {
        bulk::thread::environment env;
        env.spawn(P, [](bulk::world& world) {
            auto s = world.rank();
            auto p = world.active_processors();

            auto hasSolution = findSolution({}, {0, 1, 2, 3, 4, 5, 6, 7});
            if (hasSolution) {
                auto solution = *hasSolution;
                for (auto v : solution) {
                    std:: cout << v << ", ";
                }
                std::cout << '\n';
                std::cout << "By " << s << " / " << p << '\n';
            }
        });
    }
};
