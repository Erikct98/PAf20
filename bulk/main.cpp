#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include <iostream>

uint32_t N;

constexpr uint32_t initValue = std::numeric_limits<uint32_t>::max();

using Board = std::vector<uint32_t>;
using RemainingList = std::vector<uint32_t>;
using BitList = std::vector<char>;

class DiagonalBitSet {
private:
    template<bool Up>
    char& getRef(uint32_t index, uint32_t val) {
        if constexpr (Up) {
            return m_listUp[val - index + m_off];
        } else {
            return m_listDown[index + val];
        }
    }

    char& getRefUp(uint32_t index, uint32_t val) {
        return getRef<true>(index, val);
    }

    char& getRefDown(uint32_t index, uint32_t val) {
        return getRef<false>(index, val);
    }

    uint32_t m_off;
    BitList m_listUp;
    BitList m_listDown;

public:
    explicit DiagonalBitSet(uint32_t size) :
            m_off(size - 1),
            m_listUp(2 * size - 1, false),
            m_listDown(2 * size - 1, false) {
    }

    bool hasInterference(uint32_t index, uint32_t val) {
        return getRefUp(index, val) || getRefDown(index, val);
    }

    void set(uint32_t index, uint32_t val) {
        getRefUp(index, val) = true;
        getRefDown(index, val) = true;
    }

    void reset(uint32_t index, uint32_t val) {
        getRefUp(index, val) = false;
        getRefDown(index, val) = false;
    }
};

std::optional<Board> findSolution(const Board& partial, RemainingList remaining) {
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

int main(int argc, char** argv) {
    if (argc > 1) {
        printf("%s\n", argv[1]);
        N = std::strtoul(argv[1], nullptr, 10);
    } else {
        printf("What size chessboard do we use?\n");
        fflush(stdout);
        if (scanf("%ul", &N) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
    }

    bulk::thread::environment env;
    env.spawn(env.available_processors(), [](bulk::world& world) {
        bulk::var<int> var(world, 0);

        var.broadcast(-3);

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
    return 0;
}
