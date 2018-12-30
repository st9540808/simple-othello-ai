#include "search.h"
#include <limits>

int eval(Bitboard self, Bitboard opponent, Bitboard self_moves, Bitboard opp_moves)
{
    static constexpr Bitboard corners = 0x8100000000000081ULL;
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

    if (depth == 0 || (moves == 0 && opp_moves == 0)) {
        return eval(node.get_board(player), node.get_board(player ^ 1), 
                    moves, opp_moves);
    };
    if (moves == 0) {
        return alphabeta(node, depth-1, alpha, beta,
                         player^1, maximizingPlayer^1, nullptr);
    }

    if (maximizingPlayer) {
        std::vector<square> moves_arr = Othello::convert_moves_to_vector(moves);
        int max_score = std::numeric_limits<int>::min();
        int score;
        
        for (square move : moves_arr) {
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
        std::vector<square> moves_arr = Othello::convert_moves_to_vector(moves);
        int min_score = std::numeric_limits<int>::max();
        int score;

        for (square move : moves_arr) {
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