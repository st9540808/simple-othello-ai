#include <iostream>
#include <string>
#include <limits>
#include "othello.h"
#include "search.h"

Othello::Othello(void)
    : boards{0, 0}
{
    set_square_state(SQ_D5, STAT_BLACK);
    set_square_state(SQ_E4, STAT_BLACK);
    set_square_state(SQ_D4, STAT_WHITE);
    set_square_state(SQ_E5, STAT_WHITE);
}

Bitboard Othello::get_board(color player) const
{
    return boards[player];
}

Bitboard Othello::get_white_board() const
{
    return boards[WHITE];
}

Bitboard Othello::get_black_board() const
{
    return boards[BLACK];
}

Bitboard Othello::get_occupancy() const
{
    return (boards[WHITE] | boards[BLACK]);
}

Bitboard Othello::get_empty() const
{
    return ~(get_occupancy());
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

    assert((boards[BLACK] & boards[WHITE]) == 0 &&
           "black board and white board should be disjoint");
    
    if ((boards[BLACK] & mask) != 0)
        return state::STAT_BLACK;
    else if ((boards[WHITE] & mask) != 0)
        return state::STAT_WHITE;
    return state::STAT_EMPTY; 
}

Bitboard Othello::generate_moves_for(color player) const
{
    color opponent = static_cast<color>(player ^ 1);
    return ::generate_moves(boards[player], boards[opponent]);
}

std::vector<square> Othello::convert_moves_to_vector(Bitboard moves)
{
    const int num_moves = popcount(moves);
    std::vector<square> moves_arr(num_moves, SQ_NONE);
    Bitboard x = moves;

    for (int i = 0; i < num_moves; i++) {
        moves_arr[i] = static_cast<square>(__builtin_ctzll(x)); // make square
        x &= x - 1; // reset LS1B
    }
    assert(x == 0);
    return moves_arr;
}

void Othello::convert_moves_to_arr(Bitboard moves, square* moves_arr, int num_moves)
{
    Bitboard x = moves;

    for (int i = 0; i < num_moves; i++) {
        moves_arr[i] = static_cast<square>(__builtin_ctzll(x));
        x &= x - 1;
    };
    assert(x == 0);
}

bool Othello::has_valid_move(color player)
{
    return generate_moves_for(player) != 0;
}

bool Othello::is_valid_move(color player, square pos)
{
    Bitboard mask = make_bitboard(pos);
    assert(SQ_A1 <= pos && pos <= SQ_H8);
    return (generate_moves_for(player) & mask) != 0;
}

void Othello::make_move(color player, square pos)
{
    color opponent = static_cast<color>(player ^ 1);
    ::resolve_move(&boards[player], &boards[opponent], pos);
}

void Othello::play(void)
{
    std::string input_move;
    Bitboard moves;
    square s;

    for (color turn = BLACK; ; turn ^= 1) {
        std::printf("turn: %s\n", turn == BLACK ? "black" : "white");
        moves = generate_moves_for(turn);
        print_board_with_moves(moves);

        do {
            std::getline(std::cin, input_move);
            input_move[0] |= 1 << 5;
            s = make_square(input_move[1]-'1', input_move[0]-'a');
        } while ((moves & s) == 0);
        make_move(turn, s);
    }
}

void Othello::play_with_ai(color player)
{
    std::string input_move;
    Bitboard moves;
    square s;

    for (color turn = BLACK; ; turn ^= 1) {
        std::printf("turn: %s\n", turn == BLACK ? "black" : "white");
        
        if (generate_moves_for(turn) == 0 && generate_moves_for(turn ^ 1) == 0) {
            // game over
            break;
        } else if (generate_moves_for(turn) == 0) {
            // no moves for current player, pass
            continue;
        }

        if (turn == player) {
            moves = generate_moves_for(turn);
            print_board_with_moves(moves);
            do {
                std::getline(std::cin, input_move);
                input_move[0] |= 1 << 5;
                s = make_square(input_move[1]-'1', input_move[0]-'a');
            } while ((moves & s) == 0);
            
            make_move(turn, s);
        } else {
            Othello snapshot = *this;
            s = alphabeta(snapshot, turn, 12);
            snapshot.make_move(turn, s);
            snapshot.set_square_state(s, STAT_EMPTY);
            snapshot.print_board_with_moves(1ull << s);
            
            make_move(turn, s);
        }
    }
}

void Othello::ai_self_play(color player)
{
    square s;
    int black_score, white_score;

    for (color turn = BLACK; ; turn ^= 1) {
        if (generate_moves_for(turn) == 0 && generate_moves_for(turn ^ 1) == 0) {
            // game over
            std::puts("game over");
            break;
        } else if (generate_moves_for(turn) == 0) {
            // no moves for current player, pass
            continue;
        }

        std::printf("turn: %s\n", turn == BLACK ? "black" : "white");

        if (turn == player) {
            Othello snapshot = *this;
            s = alphabeta(snapshot, turn, 3);
            snapshot.make_move(turn, s);
            snapshot.set_square_state(s, STAT_EMPTY);
            snapshot.print_board_with_moves(1ull << s);
            
            make_move(turn, s);
        } else {
            Othello snapshot = *this;
            s = negamax(snapshot, turn, 7);
            snapshot.make_move(turn, s);
            snapshot.set_square_state(s, STAT_EMPTY);
            snapshot.print_board_with_moves(1ull << s);
            
            make_move(turn, s);
        }
    }

    black_score = popcount(boards[BLACK]);
    white_score = popcount(boards[WHITE]);
    std::printf("result: BLACK %d - %d WHITE\n", black_score, white_score);
    if (black_score == white_score)
        std::puts("draw");
    else
        std::printf("%s wins\n", black_score > white_score ? "BLACK" : "WHITE");
}

bool Othello::ai_play_with_rand(color player)
{
    square s;
    int black_score, white_score;

    for (color turn = BLACK; ; turn ^= 1) {
        if (generate_moves_for(turn) == 0 && generate_moves_for(turn ^ 1) == 0) {
            // game over
            break;
        } else if (generate_moves_for(turn) == 0) {
            // no moves for current player, pass
            continue;
        }

        // std::printf("turn: %s\n", turn == BLACK ? "black" : "white");

        if (turn == player) {
            Othello snapshot = *this;
            // s = negamax(snapshot, turn, 3, mobility_eval_for_player);
            s = alphabeta(snapshot, turn, 3, mobility_eval_for_player);
            // snapshot.make_move(turn, s);
            // snapshot.set_square_state(s, STAT_EMPTY);
            // snapshot.print_board_with_moves(1ull << s);
            
            make_move(turn, s);
        } else {
            Othello snapshot = *this;
            s = rand_generate(snapshot, turn);
            // snapshot.make_move(turn, s);
            // snapshot.set_square_state(s, STAT_EMPTY);
            // snapshot.print_board_with_moves(1ull << s);
            
            make_move(turn, s);
        }
    }

    black_score = popcount(boards[BLACK]);
    white_score = popcount(boards[WHITE]);
    // std::printf("result: BLACK %d - %d WHITE\n", black_score, white_score);
    if (black_score == white_score)
        std::puts("draw");
    else
        std::printf("%s wins\n", black_score > white_score ? "BLACK" : "WHITE");
    return popcount(boards[player]) > popcount(boards[player^1]);
}

void Othello::print_board(void)
{
    Bitboard mask;

    assert((boards[BLACK] & boards[WHITE]) == 0 &&
           "black board and white board should be disjoint");

    std::puts("  A B C D E F G H");
    for (int i = 0; i < 8; i++) {
        std::printf("%c ", '1' + i);
        for (int j = 0; j < 8; j++) {
            mask = make_bitboard(i, j);
            if (boards[WHITE] & mask)
                std::printf("%c ", 'W');
            else if (boards[BLACK] & mask)
                std::printf("%c ", 'B');
            else
                std::printf("%c ", '.');
        }
        std::puts("");
    }
    std::puts("");
}

void Othello::print_board_with_moves(Bitboard moves)
{
    Bitboard mask;

    assert((boards[BLACK] & boards[WHITE]) == 0 &&
           "black board and white board should be disjoint");
    assert(((boards[BLACK] | boards[WHITE]) & moves) == 0 &&
           "boards and moves should be disjoint");

    std::puts("  A B C D E F G H");
    for (int i = 0; i < 8; i++) {
        std::printf("%c ", '1' + i);
        for (int j = 0; j < 8; j++) {
            mask = make_bitboard(i, j);
            if (boards[WHITE] & mask)
                std::printf("%c ", 'W');
            else if (boards[BLACK] & mask)
                std::printf("%c ", 'B');
            else if (moves & mask)
                std::printf("%c ", '+');
            else
                std::printf("%c ", '.');
        }
        std::puts("");
    }
    std::puts("");
}