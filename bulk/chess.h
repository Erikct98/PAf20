#ifndef PAF20_PARALLELALGORITHMS_CHESS_H
#define PAF20_PARALLELALGORITHMS_CHESS_H

using Board = std::vector<uint32_t>;

void swap(uint32_t *one, uint32_t *two) {
    uint32_t temp = *one;
    *one = *two;
    *two = temp;
}

void shuffle(std::vector<uint32_t> array) {
    size_t n = array.size();
    if (size > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            swap(&array[i], &array[j]);
        }
    }
}


#endif //PAF20_PARALLELALGORITHMS_CHESS_H
