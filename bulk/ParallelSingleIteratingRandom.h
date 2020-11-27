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

    std::optional<Board> findSolution();

public:
    void solve();
};


#endif //PAF20_PARALLELALGORITHMS_PARALLELSINGLEITERATINGRANDOM_H
