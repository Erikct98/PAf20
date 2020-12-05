#include "Board.h"

void Board::push(uint64_t elt) {
    // Update horizontal and diagonals
    horizontal |= elt;
    diagUp |= elt;
    diagDown |= elt << remaining;

    // Shift diagonals for next iteration
    diagUp <<= 1u;
    diagDown >>= 1u;

    // Add element to formation
    formation.push_back(elt);

    // Add new mask
    masks.push_back(getAvailable());

    // Update index
    idx++;
}

uint64_t Board::pop() {
    // Remove last entry from formation
    uint64_t elt = formation.back();
    formation.pop_back();

    // Shift diagonals back
    diagUp >>= 1u;
    diagDown <<= 1u;

    // Update horizontal and diagonals
    uint64_t mask = ~elt;
    horizontal &= mask;
    diagUp &= mask;
    diagDown &= ~(elt << remaining);

    // Remove mask
    masks.pop_back();

    // Update index
    idx--;

    // Return element
    return elt;
}

uint64_t Board::getNextElt() {
    uint64_t mask = masks.back();

    // Get elt at LSB
    // TODO: make faster / without loop
    uint64_t elt = 1u;
    while ((elt & mask) == 0 && elt > 0) {
        elt <<= 1u;
    }

    // Update mask
    masks.pop_back();
    uint64_t newMask = mask & ~elt & defMask;
    masks.push_back(newMask);

    // Return element
    return elt;
}

uint64_t Board::getAvailable() const {
    return (~(diagUp | (diagDown >> remaining) | horizontal)) & defMask;
}

void Board::reset() {
    idx = 0;
    diagDown = 0;
    diagUp = 0;
    horizontal = 0;
    formation.erase(formation.begin(), formation.end());
}

uint64_t Board::getIdx() const {
    return idx;
}


