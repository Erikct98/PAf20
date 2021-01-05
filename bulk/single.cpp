#include "ParallelSingleIteratingRandom.h"
#include "ParallelSingleRecursiveRandom.h"
#include <iostream>

template<typename T>
struct AlgorithmName;

#define REGISTER_PARSE_TYPE(type) \
    template<> \
    struct AlgorithmName<type> { \
        static const char* name; \
    }; \
    const char* AlgorithmName<type>::name = #type


REGISTER_PARSE_TYPE(ParallelSingleIteratingRandom);
REGISTER_PARSE_TYPE(ParallelSingleRecursiveRandom);

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


    ParallelSingleRecursiveRandom psir(N, P);
    std::cout << N << ';' << P << ';';
    auto begin = std::chrono::steady_clock::now();
    psir.solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << '\n';

    return 0;
}
