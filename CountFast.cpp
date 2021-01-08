#ifndef USE_THREAD
#include <bulk/backends/mpi/mpi.hpp>
#else
#include <bulk/backends/thread/thread.hpp>
#endif
#include <bulk/coarray.hpp>
#include <bulk/algorithm.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>


class Board {
private:
    uint64_t size, remaining, idx;
    uint64_t defMask;
    uint64_t diagUp, diagDown, horizontal;
    std::vector<uint64_t> formation;
    std::vector<uint64_t> masks;
public:
    explicit Board(uint32_t size) :
            size(size),
            remaining(64 - size),
            defMask((1u << size) - 1),
            idx(0),
            diagUp(0),
            diagDown(0),
            horizontal(0){
        formation.reserve(size);
        masks.reserve(size);
        masks.push_back(defMask);
    }

    void push(uint64_t elt);
    uint64_t pop();
    uint64_t getNextElt();

    void reset();
    uint64_t getIdx() const;
    uint64_t getAvailable() const;

};

void Board::push(uint64_t elt) {
    // Update horizontal and diagonals
    horizontal |= elt;
    diagUp |= elt;
    diagDown |= elt << remaining;

    // Shift diagonals for next iteration
    diagUp <<= 1u;
    diagDown >>= 1u;

    // Add element to formation
    formation.push_back(elt);

    // Add new mask
    masks.push_back(getAvailable());

    // Update index
    idx++;
}

uint64_t Board::pop() {
    // Remove last entry from formation
    uint64_t elt = formation.back();
    formation.pop_back();

    // Shift diagonals back
    diagUp >>= 1u;
    diagDown <<= 1u;

    // Update horizontal and diagonals
    uint64_t mask = ~elt;
    horizontal &= mask;
    diagUp &= mask;
    diagDown &= ~(elt << remaining);

    // Remove mask
    masks.pop_back();

    // Update index
    idx--;

    // Return element
    return elt;
}

uint64_t Board::getNextElt() {
    uint64_t mask = masks.back();

    uint64_t elt = mask & -mask;

    // Update mask
    masks.back() &= ~elt & defMask;

    // Return element
    return elt;
}

uint64_t Board::getAvailable() const {
    return (~(diagUp | (diagDown >> remaining) | horizontal)) & defMask;
}

void Board::reset() {
    idx = 0;
    diagDown = 0;
    diagUp = 0;
    horizontal = 0;
    formation.clear();
    masks.clear();
    masks.push_back(defMask);
}

uint64_t Board::getIdx() const {
    return idx;
}


using RemainingList = std::vector<uint32_t>;

class CountIterating {
public:
    uint64_t solve();

    CountIterating(uint32_t n, uint32_t procs);

private:
    uint32_t countSolutions(Board &board) const;

    uint32_t N;
    uint32_t procs;
};

bool verify(uint64_t val, uint32_t N) {
    static std::map<uint32_t, uint64_t> validCountsAll = {
            {1, 1},
            {2, 0},
            {3, 0},
            {4, 2},
            {5, 10},
            {6, 4},
            {7, 40},
            {8, 92},
            {9, 352},
            {10, 724},
            {11, 2680},
            {12, 14200},
            {13, 73712},
            {14, 365596},
            {15, 2279184},
            {16, 14772512},
            {17, 95815104},
            {18, 666090624},
            {19, 4968057848},
            {20, 39029188884},
            {21, 314666222712},
            {22, 2691008701644},
            {23, 24233937684440},
            {24, 227514171973736},
            {25, 2207893435808352},
            {26, 22317699616364044},
            {27, 234907967154122528},
    };

    auto it = validCountsAll.find(N);
    if (it != validCountsAll.end()) {
        return it->second == val;
    }
    return false;
}

template<typename T>
struct AlgorithmName;

#define REGISTER_PARSE_TYPE(type) \
    template<> \
    struct AlgorithmName<type> { \
        static const char* name; \
    }; \
    const char* AlgorithmName<type>::name = #type


REGISTER_PARSE_TYPE(CountIterating);

#define GET_NAME(type) AlgorithmName<decltype(type)>::name

int main(int argc, char** argv) {
    uint32_t N = 0, P = 0;
    if (argc > 2) {
        N = std::strtoul(argv[1], nullptr, 10);
        P = std::strtoul(argv[2], nullptr, 10);
    } else {
        std::cout << "What size chessboard do we use?\n" << std::flush;
        std::cin >> N;
        if (!std::cin) {
            std::cout << "Invalid size!\n";
            return EXIT_FAILURE;
        }
        std::cout << "How many processors are we using?\n" << std::flush;
        std::cin >> P;
        if (!std::cin || P == 0 || N == 0) {
            std::cout << "Invalid size!\n";
            return EXIT_FAILURE;
        }
    }

    std::cout << "Running: board=" << N << " p=" << P << std::endl;


    CountIterating psir(N, P);
    auto begin = std::chrono::steady_clock::now();
    uint64_t count = psir.solve();
    auto end = std::chrono::steady_clock::now();
    if (!verify(count, N)) {
        std::cout << "INVALID VALUE! got: " << count << '\n';
    } else {
        std::cout << GET_NAME(psir) << " for N=" << N << " P=" << P << " correct in " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " micro s (" << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << " s)\n";
    }

    return 0;
}


CountIterating::CountIterating(uint32_t n, uint32_t procs) : N(n), procs(procs) {}

uint32_t CountIterating::countSolutions(Board &board) const {
    uint32_t count = 0;
    uint64_t endIdx = board.getIdx() - 1;// Dirty hack, but it works

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

//#define TIME_INDIVIDUAL

#ifdef TIME_INDIVIDUAL
#define WHEN_TIMING(x) x
#else
#define WHEN_TIMING(x)
#endif

uint64_t CountIterating::solve() {

    if (N <= 1) {
        return 1;
    }
    if (N <= 3) {
        return 0;
    }

    // N is at least 4

    WHEN_TIMING(auto begin = std::chrono::steady_clock::now();)

#ifndef USE_THREAD
    bulk::mpi::environment env;
#else
    bulk::thread::environment env;
#endif

    uint64_t val;
    env.spawn(procs, [&](bulk::world &world) {
      uint32_t s = world.rank();
      uint32_t p = world.active_processors();

      WHEN_TIMING(uint32_t realCase = 0;)

      // Compute number of cases
      uint32_t depth = 1;
      uint32_t cases = (N + 1) / 2;

      uint32_t fullCases = (N + 1) / 2;
      std::vector<uint32_t> startFactors;
      startFactors.push_back(1);

      while (p * p > cases && depth < N) {
          startFactors.push_back(fullCases);
          cases *= N - depth;
          fullCases *= N;
          depth++;
      }

//        world.log_once("Using depth: %d, %d, %d, %d", depth, startFactors.size(), cases, fullCases);
//
      std::vector<uint32_t> startIndices(depth, -1);

      // Init board
      Board board{N};
      std::vector<uint64_t> placeHolder(N);
      bool isOdd = N % 2 == 1;
      uint32_t halfWay = (N + 1) / 2;

      // Count all solutions
      auto count = bulk::var<uint64_t>(world, 0u);
      auto perCount = bulk::coarray<uint64_t>(world, fullCases, 0lu);

      for (uint32_t caseNr = s; caseNr < cases; caseNr += p) {
          board.reset();

          // Create placeholder
          for (uint32_t i = 0; i < N; i++) placeHolder[i] = i;

          // Setup case
          uint32_t i = caseNr, j, idx;
          for (j = 0; j < depth; j++) {
              if (j == 0) {
                  idx = i % halfWay;
                  i /= halfWay;
              } else {
                  idx = i % (N - j);
                  i /= (N - j);
              }

              uint64_t available = board.getAvailable();
              unsigned long &value = placeHolder[j + idx];
              startIndices[j] = value;
              if ((available & (1u << value)) == 0) break;
              board.push(1u << value);
              std::swap(placeHolder[j], value);
          }
          if (j != depth
              || (isOdd && depth > 1 && placeHolder[0] == halfWay - 1 && placeHolder[1] > placeHolder[0])) {
              continue;// Diagonal clash, or unnecessary count
          }

          // Count number of solutions
          uint64_t solutions = countSolutions(board);
          uint32_t index = std::inner_product(startIndices.begin(), startIndices.end(), startFactors.begin(), 0u);

//            world.log("%d found %07lu for %d, %d idx: %d", s, solutions, startIndices[0], startIndices[1], index);

//            if (isOdd && caseNr % halfWay == halfWay - 1) {
//                perCount[caseNr % halfWay] += solutions;
//            } else {
//                perCount[caseNr % halfWay] += solutions;
//                perCount[N - 1 - (caseNr % halfWay)] += solutions;
//            }
          perCount[index] += 2 * solutions;
//                perCount[caseNr % halfWay + halfWay] += solutions;
//                count += 2 * solutions;
          WHEN_TIMING(realCase++;)
      }

      WHEN_TIMING(
              auto end = std::chrono::steady_clock::now();
              auto duration = end - begin;
              uint64_t countMe = std::accumulate(perCount.begin(), perCount.end(), uint64_t(0));
              world.log("%02ud had %ud, [%u] real cases (in %07ld ms, %d s)", s, realCase, countMe, static_cast<float>(cases) / p, std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), std::chrono::duration_cast<std::chrono::seconds>(duration).count());

              auto durVar = bulk::var<decltype(duration.count())>(world, std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
              auto minDur = bulk::min(durVar);
              auto maxDur = bulk::max(durVar);

              if (s == 0) {
                  world.log("Min: %d, Max: %d = %f", minDur, maxDur, static_cast<float>(maxDur) / static_cast<float>(minDur));
              })

      // Exchange counts
      auto res = bulk::foldl_each(
              perCount, [](auto &lhs, auto &rhs) { lhs += rhs; }, uint64_t(0));

      uint64_t totalCount = std::accumulate(res.begin(), res.end(), uint64_t(0));

      if (s == 0) {
          val = totalCount;
//            uint32_t i = 0;
//            for (auto r : res) {
////                printf("For Q0 = %02d got %d\n", i++, r);
////                std::cout << "For Q0 = " << i++ << " got " << r << " results\n";
//            }
      }
    });

    return val;
}
