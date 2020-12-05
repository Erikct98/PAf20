#pragma once

#include "Board.h"

using RemainingList = std::vector<uint32_t>;

class CountIterating {
public:
    void solve();

    CountIterating(uint32_t n, uint32_t procs);

private:
    uint32_t countSolutions(Board &board) const;

    uint32_t N;
    uint32_t procs;
};