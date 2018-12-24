#include "bitboard.h"

void print_board(uint64_t b)
{
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            if (b & (1ull << (i*8 + j))) std::putchar('1');
            else std::putchar('.');
        }
        std::putchar('\n');
    }
}

