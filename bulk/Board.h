#ifndef PAF20_PARALLELALGORITHMS_BOARD_H
#define PAF20_PARALLELALGORITHMS_BOARD_H

#include <cstdint>
#include <iostream>
#include <vector>


class Board {
private:
    uint64_t size, remaining, idx;
    uint64_t defMask;
    uint64_t diagUp, diagDown, horizontal;
    std::vector<uint64_t> formation;
    std::vector<uint64_t> masks;
public:
    explicit Board(uint32_t size) :
        size(size),
        remaining(64 - size),
        defMask((1u << size) - 1),
        idx(0),
        diagUp(0),
        diagDown(0),
        horizontal(0){
      formation.reserve(size);
      masks.reserve(size);
      masks.push_back(defMask);
    }

    void push(uint64_t elt);
    uint64_t pop();
    uint64_t getNextElt();

    void reset();
    uint64_t getIdx() const;
    uint64_t getAvailable() const;


};


#endif //PAF20_PARALLELALGORITHMS_BOARD_H
