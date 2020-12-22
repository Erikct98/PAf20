#pragma once

#include "DiagonalBitSet.h"
#include "chess.h"
#include <bulk/coarray.hpp>

using RemainingList = std::vector<uint32_t>;

class CountRecursive {
public:
    uint64_t solve();

    CountRecursive(uint32_t n, uint32_t procs);

private:
    uint64_t countSolutions(std::vector<uint32_t> &board, uint32_t idx, DiagonalBitSet &diag, bulk::coarray<uint64_t>& counts);

    uint32_t N;
    uint32_t procs;
};
