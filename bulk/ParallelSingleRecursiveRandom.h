#ifndef PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H
#define PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H

#include <bulk/backends/thread/environment.hpp>
#include <deque>
#include <iostream>
#include <cstdint>
#include "chess.h"
#include "DiagonalBitSet.h"

class ParallelSingleRecursiveRandom {
private:
    uint32_t N;
    uint32_t P;

    uint32_t findSolution(bulk::world &world, std::vector<uint32_t> &board, uint32_t idx, DiagonalBitSet &diag);

public:
    void solve();
    ParallelSingleRecursiveRandom(int N, int P) : N(N), P(P) {};
};

#endif //PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H
