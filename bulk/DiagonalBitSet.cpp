#include "DiagonalBitSet.h"

char& DiagonalBitSet::getRefUp(uint32_t index, uint32_t val) {
    return m_listUp[val - index + m_off];
}

char& DiagonalBitSet::getRefDown(uint32_t index, uint32_t val) {
    return m_listDown[index + val];
}

bool DiagonalBitSet::hasInterference(uint32_t index, uint32_t val) {
    return getRefUp(index, val) || getRefDown(index, val);
}

void DiagonalBitSet::set(uint32_t index, uint32_t val) {
    getRefUp(index, val) = true;
    getRefDown(index, val) = true;
}

void DiagonalBitSet::reset(uint32_t index, uint32_t val) {
    getRefUp(index, val) = false;
    getRefDown(index, val) = false;
}
