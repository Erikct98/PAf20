#ifndef PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H
#define PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H

#include "DiagonalBitSet.h"
#include "chess.h"
#include <bulk/variable.hpp>

class ParallelSingleRecursiveRandom {
private:
    uint32_t N;
    uint32_t P;
    uint32_t SEED;

    bool findSolution(bulk::world &world, std::vector<uint32_t> &board, uint32_t idx, DiagonalBitSet &diag,
                          bulk::var<bool>& done, uint32_t& iterCount);

public:
    const uint32_t SYNC_THRESHOLD = 1000;

    void solve();
    ParallelSingleRecursiveRandom(int N, int P);
    ParallelSingleRecursiveRandom(int N, int P, int SEED) : N(N), P(P), SEED(SEED){};
};

#endif //PAF20_PARALLELALGORITHMS_PARALLELSINGLERECURSIVERANDOM_H
