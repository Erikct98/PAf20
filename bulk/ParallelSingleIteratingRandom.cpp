#include "ParallelSingleIteratingRandom.h"
#include "DiagonalBitSet.h"

std::optional<Board> ParallelSingleIteratingRandom::findSolution(const Board &partial, RemainingList remaining) {
    // Check sizes
    const uint32_t upto = partial.size();
    if (remaining.size() + upto != N) {
        std::cout << "Failure!\n";
        return {};
    }

    // Setup board
    Board board(N);
    for (uint32_t i = 0; i < upto; ++i) {
        board[i] = partial[i];
    }
    for (uint32_t i = 0; i < remaining.size(); ++i) {
        board[upto + i] = remaining[i];
    }

    // Setup diagonals
    DiagonalBitSet diag{N};
    for (uint32_t i = 0; i < upto; ++i) {
        diag.set(i, board[i]);
    }

    // Setup picked state
    std::vector<uint32_t> picks;
    picks.reserve(remaining.size());

    uint32_t currIdx = upto;
    uint32_t remainingIdx;
    while (currIdx < N) {
        remainingIdx = currIdx;
        while (remainingIdx < N && diag.hasInterference(currIdx, board[remainingIdx])) {
            ++remainingIdx;
        }
        if (remainingIdx >= N) {
            currIdx--;
            if (currIdx < upto) {
                return {};
            }
            remainingIdx = picks.back();
            picks.pop_back();
            diag.reset(currIdx, board[currIdx]);
            swap(&board[currIdx], &board[remainingIdx]);
        } else {
            swap(&board[currIdx], &board[remainingIdx]);
            diag.set(currIdx, board[currIdx]);
            picks.push_back(remainingIdx);
            currIdx++;
        }
    }

    return board;
}

void ParallelSingleIteratingRandom::solve() {
    bulk::thread::environment env;
    env.spawn(P, [&](bulk::world &world) {
        auto s = world.rank();
        auto p = world.active_processors();

        RemainingList range(N);
        for (uint32_t i = 0; i < N; i++) range[i] = i;
        shuffle(range);

        auto hasSolution = findSolution({}, range);
        if (hasSolution) {
            auto solution = *hasSolution;
            for (auto v : solution) {
                std::cout << v << ", ";
            }
            std::cout << '\n';
            std::cout << "By " << s << " / " << p << '\n';
        }
    });
}