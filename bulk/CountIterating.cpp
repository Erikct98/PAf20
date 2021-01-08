#include "CountIterating.h"
#include <bulk/backends/thread/environment.hpp>

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

#define TIME_INDIVIDUAL

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
    bulk::thread::environment env;
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
            if (j != depth || (isOdd && depth > 1 && placeHolder[0] == halfWay - 1 && placeHolder[1] > placeHolder[0])) continue;

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
