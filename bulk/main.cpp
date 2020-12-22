#include "CountIterating.h"
#include "CountRecursive.h"
#include "ParallelSingleRecursiveRandom.h"
#include <chrono>
#include <iostream>
#include <map>

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


REGISTER_PARSE_TYPE(CountRecursive);
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


    CountRecursive psir(N, P);
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
