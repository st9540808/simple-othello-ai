#include "bitboard.h"

Bitboard white;

void print_board(Bitboard b)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (b & (1ull << (i*8 + j))) std::putchar('1');
            else std::putchar('.');
        }
        std::putchar('\n');
    }
}
