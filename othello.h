#ifndef OTHELLO_H
#define OTHELLO_H

#include <iostream>
#include <ostream>
#include <vector>
#include "bitboard.h"

class Othello {
public:
    Othello(void);
    Bitboard get_board(color player) const;
    Bitboard get_white_board() const;
    Bitboard get_black_board() const;
    Bitboard get_occupancy() const;
    Bitboard get_empty() const;

    state get_square_state(square);
    void set_square_state(square, state);
    
    Bitboard generate_moves_for(color player) const;
    static std::vector<square> convert_moves_to_vector(Bitboard moves);
    static void convert_moves_to_arr(Bitboard moves, square* moves_arr, int num_moves);
    bool has_valid_move(color player);
    bool is_valid_move(color player, square pos);
    void make_move(color player, square pos);
    void play(void);
    void play_with_ai(color player);
    void ai_self_play(color player = BLACK);
    bool ai_play_with_rand(color player = BLACK);
    
    void print_board(void);
    void print_board_with_moves(Bitboard moves);
private:
    Bitboard boards[num_colors];
};

#endif