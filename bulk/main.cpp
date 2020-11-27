#include "ParallelSingleIteratingRandom.h"

int main(int argc, char** argv) {
    uint32_t N = 0, P = 0;
    if (argc > 2) {
        printf("%s\n", argv[1]);
        N = std::strtoul(argv[1], nullptr, 10);
        P = std::strtoul(argv[2], nullptr, 10);
    } else {
        printf("What size chessboard do we use?\n");
        fflush(stdout);
        if (scanf("%ul", &N) != 1) {
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
        printf("How many processors are we using?\n");
        fflush(stdout);
        if (scanf("%ul", &P) != 1) { // FIXME: this is unsafe.
            printf("Invalid size!");
            return EXIT_FAILURE;
        }
    }

    ParallelSingleIteratingRandom psir(N, P);
    psir.solve();

    return 0;
}
