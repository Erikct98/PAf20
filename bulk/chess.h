#ifndef PAF20_PARALLELALGORITHMS_CHESS_H
#define PAF20_PARALLELALGORITHMS_CHESS_H

#include <utility>
#include <vector>
#include <inttypes.h>
#include <random>

using Board = std::vector<uint32_t>;

inline void shuffle(std::vector<uint32_t> array) {
    size_t n = array.size();
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            std::swap(array[i], array[j]);
        }
    }
}


#endif //PAF20_PARALLELALGORITHMS_CHESS_H
