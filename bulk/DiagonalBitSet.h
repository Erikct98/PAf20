#ifndef PAF20_PARALLELALGORITHMS_DIAGONALBITSET_H
#define PAF20_PARALLELALGORITHMS_DIAGONALBITSET_H

#include <vector>
#include <stdint.h>

using BitList = std::vector<char>;

class DiagonalBitSet {
private:

    char& getRefUp(uint32_t index, uint32_t val);
    char& getRefDown(uint32_t index, uint32_t val);

    uint32_t m_off;
    BitList m_listUp;
    BitList m_listDown;

public:
    explicit DiagonalBitSet(uint32_t size) :
            m_off(size - 1),
            m_listUp(2 * size - 1, false),
            m_listDown(2 * size - 1, false) {
    }

    bool hasInterference(uint32_t index, uint32_t val);
    void set(uint32_t index, uint32_t val);
    void reset(uint32_t index, uint32_t val);
};


#endif //PAF20_PARALLELALGORITHMS_DIAGONALBITSET_H
