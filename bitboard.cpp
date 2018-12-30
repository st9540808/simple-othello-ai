#include "bitboard.h"

void print_bitboard(Bitboard b)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (b & (1ull << (i*8 + j))) std::printf("1 ");
            else std::printf(". ");
        }
        std::putchar('\n');
    }
}

Bitboard shift(Bitboard board, int dir)
{
    static constexpr Bitboard avoidwrap[8] = {
        0x7F7F7F7F7F7F7F7FULL, /* Right. */
        0x007F7F7F7F7F7F7FULL, /* Down-right. */
        0xFFFFFFFFFFFFFFFFULL, /* Down. */
        0x00FEFEFEFEFEFEFEULL, /* Down-left. */
        0xFEFEFEFEFEFEFEFEULL, /* Left. */
        0xFEFEFEFEFEFEFE00ULL, /* Up-left. */
        0xFFFFFFFFFFFFFFFFULL, /* Up. */
        0x7F7F7F7F7F7F7F00ULL  /* Up-right. */
    };
    static constexpr int lshift[] = {
        0, /* Right. */
        0, /* Down-right. */
        0, /* Down. */
        0, /* Down-left. */
        1, /* Left. */
        9, /* Up-left. */
        8, /* Up. */
        7  /* Up-right. */
    };
    static constexpr int rshift[] = {
        1, /* Right. */
        9, /* Down-right. */
        8, /* Down. */
        7, /* Down-left. */
        0, /* Left. */
        0, /* Up-left. */
        0, /* Up. */
        0  /* Up-right. */
    };

    assert(0 <= dir && dir < num_dirs);
    if (dir < num_dirs / 2)
        return (board >> rshift[dir]) & avoidwrap[dir];
    else
        return (board << lshift[dir]) & avoidwrap[dir];
}

// Use Dumb7Fill
Bitboard generate_moves(Bitboard self, Bitboard opponent)
{
    Bitboard moves = 0, x;
    Bitboard empty = ~(self | opponent);

    assert((self & opponent) == 0 && "self board and opponent board should be disjoint");
    
    for (int dir = 0; dir < num_dirs; dir++) {
        x  = shift(self, dir) & opponent;
        x |= shift(x, dir) & opponent;
        x |= shift(x, dir) & opponent;
        x |= shift(x, dir) & opponent;
        x |= shift(x, dir) & opponent;
        x |= shift(x, dir) & opponent;
        moves |= shift(x, dir) & empty;
    }

    return moves;
}

void resolve_move(Bitboard* self, Bitboard* opponent, square s)
{
    Bitboard new_piece = make_bitboard(s);
    Bitboard captured = 0, x, bound;

    assert(SQ_A1 <= s && s <= SQ_H8);
    assert((*self & *opponent) == 0 && "self board and opponent board should be disjoint");
    assert(((*self | *opponent) & new_piece) == 0 && "target in not an empty cell");

    *self |= new_piece;
    
    for (int dir = 0; dir < num_dirs; dir++) {
        x  = shift(new_piece, dir) & *opponent;
        x |= shift(x, dir) & *opponent;
        x |= shift(x, dir) & *opponent;
        x |= shift(x, dir) & *opponent;
        x |= shift(x, dir) & *opponent;
        x |= shift(x, dir) & *opponent;
        bound = shift(x, dir) & *self;
        captured |= (bound ? x : 0);
    }

    assert(captured && "no captured, not a valid move");
    
    *self ^= captured;
    *opponent ^= captured;

    assert((*self & *opponent) == 0 && "The sets must still be disjoint.");
}