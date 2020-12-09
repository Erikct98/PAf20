#pragma once

#include "chess.h"
#include "DiagonalBitSet.h"

using RemainingList = std::vector<uint32_t>;

class CountRecursive {
public:
    uint64_t solve();

    CountRecursive(uint32_t n, uint32_t procs);

private:
    uint64_t countSolutions(std::vector<uint32_t> &board, uint32_t idx, DiagonalBitSet &diag);

    uint32_t N;
    uint32_t procs;
};
