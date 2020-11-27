#include "DiagonalBitSet.h"

class DiagonalBitSet {
private:
    template<bool Up>
    char& getRef(uint32_t index, uint32_t val) {
        if constexpr (Up) {
            return m_listUp[val - index + m_off];
        } else {
            return m_listDown[index + val];
        }
    }

    char& getRefUp(uint32_t index, uint32_t val) {
        return getRef<true>(index, val);
    }

    char& getRefDown(uint32_t index, uint32_t val) {
        return getRef<false>(index, val);
    }

public:

    bool hasInterference(uint32_t index, uint32_t val) {
        return getRefUp(index, val) || getRefDown(index, val);
    }

    void set(uint32_t index, uint32_t val) {
        getRefUp(index, val) = true;
        getRefDown(index, val) = true;
    }

    void reset(uint32_t index, uint32_t val) {
        getRefUp(index, val) = false;
        getRefDown(index, val) = false;
    }
};