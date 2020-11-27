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

    std::optional<Board> findSolution(const Board& partial, RemainingList remaining);

public:
    void solve();
    ParallelSingleIteratingRandom(int N, int P) : N(N), P(P) {};
};


#endif //PAF20_PARALLELALGORITHMS_PARALLELSINGLEITERATINGRANDOM_H
