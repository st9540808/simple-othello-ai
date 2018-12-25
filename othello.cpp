#include "othello.h"

std::ostream& operator<<(std::ostream& os, const Othello& othello)
{
    Bitboard mask;

    std::puts("  A B C D E F G H");
    for (int i = 0; i < 8; i++) {
        std::printf("%c ", '1' + i);
        for (int j = 0; j < 8; j++) {
            mask = make_bitboard(i, j);
            if (othello.boards[WHITE] & mask)
                std::printf("%c ", 'W');
            else if (othello.boards[BLACK] & mask)
                std::printf("%c ", 'B');
            else
                std::printf("%c ", '.');
        }
        std::puts("");
    }
    std::puts("");
    return os;
}

Othello::Othello()
    : boards{0, 0}
{
    set_square_state(SQ_D5, STAT_BLACK);
    set_square_state(SQ_E4, STAT_BLACK);
    set_square_state(SQ_D4, STAT_WHITE);
    set_square_state(SQ_E5, STAT_WHITE);
    operator<<(std::cout, *this);
}

void Othello::set_square_state(square s, state stat)
{
    Bitboard mask = make_bitboard(s);
    boards[BLACK] &= ~mask;
    boards[WHITE] &= ~mask;
    if (stat == STAT_BLACK)
        boards[BLACK] |= mask;
    else if (stat == STAT_WHITE)
        boards[WHITE] |= mask;
}

state Othello::get_square_state(square s)
{
    Bitboard mask = make_bitboard(s);
    return state::STAT_EMPTY;
}