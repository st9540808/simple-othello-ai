#include "search.h"
#include <limits>
#include <random>
#include <algorithm>
#include <mutex>


static constexpr Bitboard corners = 0x8100000000000081ULL;
static constexpr Bitboard x_squares = make_bitboard(SQ_B2) | SQ_G2 | SQ_B7 | SQ_G7;
static constexpr Bitboard c_squares =
      make_bitboard(SQ_B1) | SQ_G1 | SQ_A2 | SQ_H2 | SQ_A7 | SQ_H7 | SQ_B8 | SQ_G8;
static constexpr Bitboard a_squares =
      make_bitboard(SQ_C1) | SQ_F1 | SQ_A3 | SQ_H3 | SQ_A6 | SQ_H6 | SQ_C8 | SQ_F8;
static constexpr Bitboard b_squares =
      make_bitboard(SQ_D1) | SQ_E1 | SQ_A4 | SQ_H4 | SQ_A5 | SQ_H5 | SQ_D8 | SQ_E8;
static constexpr Bitboard others = ~(corners | x_squares | a_squares | b_squares);

eval2_t weak_eval = weak_eval_for_player;
eval2_t mobility_eval = mobility_eval_for_player;
eval2_t mobility_eval2 = mobility_eval2_for_player;


int simple_eval(Othello game_state, color eval_player)
{
    Bitboard self = game_state.get_board(eval_player);
    Bitboard opponent = game_state.get_board(eval_player ^ 1);
    int val = 0;
    val += (popcount(self & corners)   - popcount(opponent & corners))   * 32;
    val += (popcount(self & x_squares) - popcount(opponent & x_squares)) * -16;
    val += (popcount(self & c_squares) - popcount(opponent & c_squares)) * -8;
    val += (popcount(self & a_squares) - popcount(opponent & a_squares)) * 4;
    val += (popcount(self & b_squares) - popcount(opponent & b_squares)) * 2;
    val += (popcount(self & others)    - popcount(opponent & others)) * -1;
    return val;
}

int weak_eval_for_player(Othello game_state, color eval_player)
{
    Bitboard self = game_state.get_board(eval_player);
    Bitboard opponent = game_state.get_board(eval_player ^ 1);
    Bitboard self_moves = game_state.generate_moves_for(eval_player);
    Bitboard opp_moves = game_state.generate_moves_for(eval_player ^ 1);
    int val;

    if (self_moves == 0 && opp_moves == 0) {
        if (popcount(self) > popcount(opponent))
            return (popcount(self) - popcount(opponent)) * (1 << 10);
        else
            return -(1 << 10);
    }

    val  = (popcount(self & corners)  - popcount(opponent & corners)) * 10;
    val += (popcount(self & ~corners) - popcount(opponent & ~corners));

    return val;
}

/////////////////////////////////////////////////////////////////////////////////
// for mobility evalution
/////////////////////////////////////////////////////////////////////////////////
static int frontier(const Othello& game, color player)
{
    Bitboard b = game.get_board(player), empty = game.get_empty();
    Bitboard f = 0;
    for (int dir = 0; dir < num_dirs; dir++)
        f |= shift(b, dir) & empty;
    return popcount(f);
};
static int mobility(const Othello& game, color player)
{
    return popcount(game.generate_moves_for(player));
};

int mobility_eval_for_player(Othello game_state, color eval_player)
{
    Bitboard self = game_state.get_board(eval_player);
    Bitboard opponent = game_state.get_board(eval_player ^ 1);
    Bitboard moves = game_state.generate_moves_for(eval_player);
    square moves_arr[64];
    const int num_moves = popcount(moves);
    
    int val = 0;
    int self_mobility = num_moves;
    int opp_mobility = mobility(game_state, eval_player^1);

    Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
    for (int i = 0; i < num_moves; i++) {
        Othello snapshot = game_state;
        snapshot.make_move(eval_player, moves_arr[i]);
        opp_mobility = std::max(mobility(snapshot, eval_player^1), opp_mobility);
    }

    val += (popcount(self & corners) - popcount(opponent & corners)) * 16;
    val += (self_mobility - opp_mobility) * 4;
    val += (frontier(game_state, eval_player) - frontier(game_state, eval_player^1)) * -1;
    val += (popcount(self) - popcount(opponent));

    return val;
}

int mobility_eval2_for_player(Othello game_state, color eval_player)
{
    Bitboard self = game_state.get_board(eval_player);
    Bitboard opponent = game_state.get_board(eval_player ^ 1);
    int val = 0;

    val += (mobility(game_state, eval_player) - mobility(game_state, eval_player^1)) * 8;
    val += (frontier(game_state, eval_player) - frontier(game_state, eval_player^1)) * -2;
    
    val += (popcount(self & corners) - popcount(opponent & corners)) * 32;
    val += (popcount(self) - popcount(opponent));
    // val += simple_eval(game_state, eval_player);

    return val;
}

/////////////////////////////////////////////////////////////////////////////////
// alpha beta search
/////////////////////////////////////////////////////////////////////////////////
square alphabeta(const Othello node, const color player, int depth, eval2_t eval)
{
    square best_move;
    int best_score = alphabeta(
        node, player, &best_move,
        depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true,
        eval, player
    );
    // std::printf("best score: %d\n", best_score);
    return best_move;
}

int alphabeta(const Othello node, const color player, square* best_move,
              int depth, int alpha, int beta, bool maximizingPlayer,
              eval2_t eval, color eval_player)
{
    Bitboard moves = node.generate_moves_for(player);
    Bitboard opp_moves = node.generate_moves_for(player ^ 1);
    const int num_moves = popcount(moves);
    square moves_arr[64];

    // leaf
    if (depth == 0 || (!moves && !opp_moves)) {
        int val = eval(node, eval_player);
        
        if (!moves && !opp_moves) {
            // game over
            int self_discs = popcount(node.get_board(eval_player));
            int opp_discs = popcount(node.get_board(eval_player^1));
            if (self_discs > opp_discs)
                val += (self_discs-opp_discs) * 1024;
            else
                val += (self_discs-opp_discs-1) * 1024;
        }
        
        return val;
    }
    
    // pass
    if (!moves && opp_moves) {
        return alphabeta(node, player^1, nullptr,
                         depth-1, alpha, beta, maximizingPlayer^1,
                         eval, eval_player);
    }

    if (maximizingPlayer) {
        int max_score = std::numeric_limits<int>::min();
        int score;

        Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
        
        for (int i = 0; i < num_moves; i++) {
            square move = moves_arr[i];
            Othello child = node;

            child.make_move(player, move);
            score = alphabeta(child, player^1, nullptr,
                              depth-1, alpha, beta, maximizingPlayer^1,
                              eval, eval_player);
            if (score > max_score) {
                max_score = score;
                if (best_move) *best_move = move; // avoid null pointer dereference
                alpha = std::max(score, alpha);
                if (alpha >= beta)
                    break;
            }
        }
        return max_score;
    } else {
        int min_score = std::numeric_limits<int>::max();
        int score;

        Othello::convert_moves_to_arr(moves, moves_arr, num_moves);

        for (int i = 0; i < num_moves; i++) {
            square move = moves_arr[i];
            Othello child = node;

            child.make_move(player, move);
            score = alphabeta(child, player^1, nullptr,
                              depth-1, alpha, beta, maximizingPlayer^1,
                              eval, eval_player);
            if (score < min_score) {
                min_score = score;
                if (best_move) *best_move = move;
                beta = std::min(score, beta);
                if (alpha >= beta)
                    break;
            }
        }
        return min_score;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// negamax
/////////////////////////////////////////////////////////////////////////////////
square negamax(const Othello node, const color player, int depth, eval2_t eval)
{
    square best_move;
    int best_score = negamax(
        node, player, &best_move, eval,
        depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()
    );
    // std::printf("best score: %d\n", best_score);
    return best_move;
}

int negamax(const Othello node, const color player, square* best_move, eval2_t eval,
            int depth, int alpha, int beta)
{
    Bitboard moves = node.generate_moves_for(player);
    Bitboard opp_moves = node.generate_moves_for(player ^ 1);
    const int num_moves = popcount(moves);
    square moves_arr[64];
    int best_value, value;

    // leaf
    if (depth == 0 || (!moves && !opp_moves))
        return eval(node, player);

    // pass
    if (!moves && opp_moves) {
        return -negamax(node, player^1, nullptr, eval,
                        depth-1, -beta, -alpha);
    }

    best_value = std::numeric_limits<int>::min();
    Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
    for (int i = 0; i < num_moves; i++) {
        square move = moves_arr[i];
        Othello child = node;

        child.make_move(player, move);
        value = -negamax(child, player^1, nullptr, eval, depth-1, -beta, -alpha);
        if (value > best_value) {
            best_value = value;
            if (best_move) *best_move = move;
            alpha = std::max(value, alpha);
            if (alpha >= beta)
                break;
        }
    }
    return best_value;
}

/////////////////////////////////////////////////////////////////////////////////
// randomly generate move
/////////////////////////////////////////////////////////////////////////////////
static std::mutex m_smart;

square rand_generate(const Othello node, const color player)
{
    static std::mt19937 generator(std::random_device{}());
    
    Bitboard moves = node.generate_moves_for(player);
    const int num_moves = popcount(moves);
    square moves_arr[64];

    assert(num_moves > 0);

    Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
    std::uniform_int_distribution<> dist(0, num_moves-1);

    return moves_arr[dist(generator)];
}

square rand_generate_smart(const Othello node, const color player)
{
    static std::mt19937 generator(std::random_device{}());
    
    Bitboard moves = node.generate_moves_for(player);
    const int num_moves = popcount(moves);
    square moves_arr[64];
    int weights[64], min;

    assert(num_moves > 0);

    Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
    for (int i = 0; i < num_moves; i++) {
        Othello child = node;
        child.make_move(player, moves_arr[i]);
        weights[i] = simple_eval(child, player);
    }

    min = *std::min_element(std::begin(weights), std::begin(weights)+num_moves);
    for (int i = 0; i < num_moves; i++)
        weights[i] += -std::min(min, 0) + 1;

    std::discrete_distribution<> d(std::begin(weights), std::begin(weights)+num_moves);
    
    std::lock_guard<std::mutex> guard(m_smart);
    return moves_arr[d(generator)];
}