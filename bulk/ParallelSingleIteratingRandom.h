#ifndef PAF20_PARALLELALGORITHMS_PARALLELSINGLEITERATINGRANDOM_H
#define PAF20_PARALLELALGORITHMS_PARALLELSINGLEITERATINGRANDOM_H

#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include <iostream>
#include "chess.h"

using RemainingList = std::vector<uint32_t>;
constexpr uint32_t initValue = std::numeric_limits<uint32_t>::max();

class ParallelSingleIteratingRandom {
private:
    uint32_t N;
    uint32_t P;

    std::optional<std::vector<uint32_t>> findSolution(const std::vector<uint32_t>& partial, RemainingList remaining);

public:
    void solve();
    ParallelSingleIteratingRandom(int N, int P) : N(N), P(P) {};
};


#endif //PAF20_PARALLELALGORITHMS_PARALLELSINGLEITERATINGRANDOM_H
