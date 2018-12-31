#include "search.h"
#include <limits>
#include <random>

static color eval_player; // player to evaluate

int eval(Othello game_state)
{
    static constexpr Bitboard corners = 0x8100000000000081ULL;

    Bitboard self = game_state.get_board(eval_player);
    Bitboard opponent = game_state.get_board(eval_player ^ 1);
    Bitboard self_moves = game_state.generate_moves_for(eval_player);
    Bitboard opp_moves = game_state.generate_moves_for(eval_player ^ 1);
    int val;

    if (self_moves == 0 && opp_moves == 0) {
        if (popcount(self) > popcount(opponent))
            return  (1 << 10);
        else
            return -(1 << 10);
    }

    val  = (popcount(self & corners)  - popcount(opponent & corners)) * 10;
    val += (popcount(self & ~corners) - popcount(opponent & ~corners));

    return val;
}

square alphabeta(const Othello node, int depth, const color player)
{
    square best_move;
    eval_player = player;
    int best_score = alphabeta(
        node, depth,
        std::numeric_limits<int>::min(), std::numeric_limits<int>::max(),
        player, true, &best_move
    );
    std::printf("best score: %d\n", best_score);
    return best_move;
}

int alphabeta(const Othello node, int depth, int alpha, int beta,
              const color player, bool maximizingPlayer, square* best_move)
{
    Bitboard moves = node.generate_moves_for(player);
    Bitboard opp_moves = node.generate_moves_for(player ^ 1);
    const int num_moves = popcount(moves);
    square moves_arr[64];

    // leaf
    if (depth == 0 || (!moves && !opp_moves))
        return eval(node);
    
    // pass
    if (!moves && opp_moves) {
        return alphabeta(node, depth-1, alpha, beta,
                         player^1, maximizingPlayer^1, nullptr);
    }

    if (maximizingPlayer) {
        int max_score = std::numeric_limits<int>::min();
        int score;

        Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
        
        for (int i = 0; i < num_moves; i++) {
            square move = moves_arr[i];
            Othello child = node;

            child.make_move(player, move);
            score = alphabeta(child, depth-1, alpha, beta, player^1, false, nullptr);
            if (score > max_score) {
                max_score = score;
                if (best_move) *best_move = move; // avoid null pointer dereference
                alpha = std::max(score, alpha);
            }
            if (alpha >= beta)
                break;
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
            score = alphabeta(child, depth-1, alpha, beta, player^1, true, nullptr);
            if (score < min_score) {
                min_score = score;
                if (best_move) *best_move = move;
                beta = std::min(score, beta);
            }
            if (alpha >= beta)
                break;
        }
        return min_score;
    }
}

square rand_generate(const Othello node, const color player)
{
    static std::mt19937 generator(std::random_device{}());
    Bitboard moves = node.generate_moves_for(player);
    const int num_moves = popcount(moves);
    square moves_arr[64];

    Othello::convert_moves_to_arr(moves, moves_arr, num_moves);

    assert(popcount(moves) != 0);
    std::uniform_int_distribution<> dist(0, num_moves-1);

    return moves_arr[dist(generator)];
}