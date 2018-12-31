#include "search.h"
#include <limits>
#include <random>

static color eval_player; // player to evaluate

int weak_eval(Othello game_state)
{
    return weak_eval_for_player(game_state, eval_player);
}

int weak_eval_for_player(Othello game_state, color eval_player)
{
    static constexpr Bitboard corners = 0x8100000000000081ULL;

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

int mobility_eval_for_player(Othello game_state)
{
    return mobility_eval_for_player(game_state, eval_player);
}

int mobility_eval_for_player(Othello game_state, color eval_player)
{
    static constexpr Bitboard corners = 0x8100000000000081ULL;
    static auto frontier = [](const Othello& game, color player) -> int {
        Bitboard b = game.get_board(player), empty = game.get_empty();
        Bitboard f = 0;
        for (int dir = 0; dir < num_dirs; dir++)
            f |= shift(b, dir) & empty;
        return popcount(f);
    };
    static auto mobility = [](const Othello& game, color player) -> int {
        return popcount(game.generate_moves_for(player));
    };

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
    val += (frontier(game_state, eval_player) - frontier(game_state, eval_player^1)) * -2;

    return val;
}

square alphabeta(const Othello node, const color player, int depth, eval_t eval)
{
    square best_move;
    eval_player = player;
    int best_score = alphabeta(
        node, player, &best_move, eval,
        depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true
    );
    // std::printf("best score: %d\n", best_score);
    return best_move;
}

int alphabeta(const Othello node, const color player, square* best_move, eval_t eval,
              int depth, int alpha, int beta, bool maximizingPlayer)
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
        return alphabeta(node, player^1, nullptr, eval,
                         depth-1, alpha, beta, maximizingPlayer^1);
    }

    if (maximizingPlayer) {
        int max_score = std::numeric_limits<int>::min();
        int score;

        Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
        
        for (int i = 0; i < num_moves; i++) {
            square move = moves_arr[i];
            Othello child = node;

            child.make_move(player, move);
            score = alphabeta(child, player^1, nullptr, eval,
                              depth-1, alpha, beta, maximizingPlayer^1);
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
            score = alphabeta(child, player^1, nullptr, eval,
                              depth-1, alpha, beta, maximizingPlayer^1);
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

square negamax(const Othello node, const color player, int depth, eval2_t eval)
{
    square best_move;
    eval_player = player;
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

square rand_generate_smart(const Othello node, const color player)
{

}