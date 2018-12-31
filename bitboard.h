#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <cstdio>
#include <cassert>

typedef uint64_t Bitboard;

constexpr int popcount(Bitboard b)
{
    return __builtin_popcountll(b);
}

// const object in c++ implies internal linkage
constexpr int num_squares = 64;
enum square : int {
  SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
  SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
  SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
  SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
  SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
  SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
  SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
  SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
  SQ_NONE,
};

constexpr int num_colors = 2;
enum color : int {
  WHITE, BLACK
};
constexpr int num_states = 3;
enum state : int {
  STAT_WHITE, STAT_BLACK, STAT_EMPTY
};

constexpr square make_square(int row, int col) {
    assert(0 <= row && row <= 7);
    assert(0 <= col && col <= 7);
    return static_cast<square>((row << 3) + col);
}

constexpr square make_square(Bitboard b) {
    assert(popcount(b) == 1);
    return static_cast<square>(__builtin_ctzll(b));
}

constexpr Bitboard make_bitboard(int row, int col) {
    assert(0 <= row && row <= 7);
    assert(0 <= col && col <= 7);
    return 1ull << ((row << 3) + col);
}

constexpr Bitboard make_bitboard(square s) {
    assert(SQ_A1 <= s && s <= SQ_H8);
    return 1ull << (static_cast<int>(s));
}

constexpr Bitboard operator&(Bitboard b, square s) {
    assert(s >= SQ_A1 && s <= SQ_H8);
    return b & (1ull << static_cast<int>(s));
}

constexpr Bitboard operator|(Bitboard b, square s) {
    assert(s >= SQ_A1 && s <= SQ_H8);
    return b | (1ull << static_cast<int>(s));
}

inline Bitboard operator&=(Bitboard& b, square s) {
    return b = b & s;
}

inline Bitboard operator|=(Bitboard& b, square s) {
    return b = b | s;
}

inline square operator++(square& s, int)
{
    square tmp = s;
    s = static_cast<square>(s + 1);
    return tmp;
}

inline color operator^(color c, int i)
{
    return static_cast<color>(static_cast<int>(c) ^ i);
}

inline color operator^=(color& c, int i)
{
    return c = c ^ i;
}


void print_bitboard(Bitboard b);

constexpr int num_dirs = 8;
Bitboard shift(Bitboard, int);
Bitboard generate_moves(Bitboard self, Bitboard opponent);
void resolve_move(Bitboard* self, Bitboard* opponent, square s);

#endif