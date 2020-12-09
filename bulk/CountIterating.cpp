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

//#define REFLECT_SIMPLE
//#define TIME_INDIVIDUAL
//#define PRIME_LOAD_BALANCE

#ifdef TIME_INDIVIDUAL
#define WHEN_TIMING(x) x
#else
#define WHEN_TIMING(x)
#endif

uint64_t CountIterating::solve() {
    WHEN_TIMING(auto begin = std::chrono::steady_clock::now();)
    bulk::thread::environment env;
    uint64_t val;
    env.spawn(procs, [&](bulk::world &world) {
        uint32_t s = world.rank();
        uint32_t p = world.active_processors();

        WHEN_TIMING(uint32_t realCase = 0;)

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

      // Init board
        Board board{N};
        std::vector<uint64_t> placeHolder(N);
#ifdef REFLECT_SIMPLE
        bool isOdd = N % 2 == 1;
        uint32_t halfWay = (N + 1) / 2;
#endif

        // Count all solutions
        auto count = bulk::var<uint64_t>(world, 0u);
        uint32_t ranCases = 0;

#ifdef PRIME_LOAD_BALANCE
        const int prime = 73;

        uint32_t casesToRun = (((cases % p) > s) ? 1u : 0u) + cases / p;
        uint32_t casesBeforeMe = (cases % p == 0 ? 0 : std::min(s, cases % p)) + (cases / p) * s;
        for (uint32_t caseNr = ((casesBeforeMe) * prime) % cases; ranCases < casesToRun; caseNr = (caseNr + prime) % cases) {
            ++ranCases;
#else
        for (uint32_t caseNr = s; caseNr < cases; caseNr += p) {
#endif
            // Reset board
            board.reset();

            // Create placeholder
            for (uint32_t i = 0; i < N; i++) placeHolder[i] = i;

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
                uint64_t available = board.getAvailable();
                if ((available & (1u << placeHolder[j + idx])) == 0) break;
                board.push(1u << placeHolder[j + idx]);
                std::swap(placeHolder[j], placeHolder[j + idx]);
            }
            if (j != depth) continue;// Diagonal clash -> go to next case

            // Count number of solutions
            uint64_t solutions = countSolutions(board);
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
                world.log("%02ud had %ud real cases (in %07ld ms, %d s)", s, realCase, static_cast<float>(cases) / p, std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), std::chrono::duration_cast<std::chrono::seconds>(duration).count());

                auto durVar = bulk::var<decltype(duration.count())>(world, std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
                auto minDur = bulk::min(durVar);
                auto maxDur = bulk::max(durVar);

                if (s == 0) {
                        world.log("Min: %d, Max: %d = %f", minDur, maxDur, static_cast<float>(maxDur) / static_cast<float>(minDur));
                }
        )

      // Exchange counts
      uint64_t totalCount = bulk::sum(count);

      if (s == 0) {
            val = totalCount;
        }
    });

    return val;
}
