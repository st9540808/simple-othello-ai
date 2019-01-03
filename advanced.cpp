#include "advanced.h"
#include <array>
#include <limits>
#include <algorithm>
#include <chrono>
#include <exception>
#include <atomic>

square iterative_alphabeta(const Othello node, const color player,
                           int init_depth, int eval_cnt_bound)
{
    square best_move, move;
    int eval_cnt = 0;
    int score, depth = init_depth;

    best_move = rand_generate(node, player); // eliminate compiler warning
    for (depth = init_depth; eval_cnt < eval_cnt_bound && depth < 25; depth++) {
        try {
            move = alphabeta_mo(node, player, depth, &eval_cnt, eval_cnt_bound, &score);
            best_move = move;
        } catch (const std::runtime_error& r) {
            std::printf("%s\n", r.what());
            depth--;
            break;
        }
    }
    std::printf("depth: %d\n", depth);
    return best_move;
}

square alphabeta_mo(const Othello node, const color player, int depth,
                    int* eval_cnt, int eval_cnt_bound, int* best_score, eval2_t eval)
{
    square best_move;
    int s = alphabeta_mo(
        node, player, &best_move,
        depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true,
        eval, player, eval_cnt, eval_cnt_bound
    );
    if (best_score) *best_score = s;
    return best_move;
}

int alphabeta_mo(const Othello node, const color player, square* best_move,
                 int depth, int alpha, int beta, bool maximizingPlayer,
                 eval2_t eval, color eval_player, int* eval_cnt, int eval_cnt_bound)
{
    Bitboard moves = node.generate_moves_for(player);
    Bitboard opp_moves = node.generate_moves_for(player^1);
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
        
        if (++*eval_cnt > eval_cnt_bound)
            throw std::runtime_error("timeout");
        return val;
    }
    
    // pass
    if (!moves && opp_moves) {
        return alphabeta_mo(node, player^1, nullptr,
                            depth-1, alpha, beta, maximizingPlayer^1,
                            eval, eval_player, eval_cnt, eval_cnt_bound);
    }

    if (maximizingPlayer) {
        int max_score = std::numeric_limits<int>::min();
        int score;

        Othello::convert_moves_to_arr(moves, moves_arr, num_moves);
        // do move ordering when first called
        if (best_move) move_ordering(node, eval_player, moves_arr, num_moves);

        for (int i = 0; i < num_moves; i++) {
            square move = moves_arr[i];
            Othello child = node;

            child.make_move(player, move);
            score = alphabeta_mo(child, player^1, nullptr,
                                 depth-1, alpha, beta, maximizingPlayer^1,
                                 eval, eval_player, eval_cnt, eval_cnt_bound);
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
            score = alphabeta_mo(child, player^1, nullptr,
                                 depth-1, alpha, beta, maximizingPlayer^1,
                                 eval, eval_player, eval_cnt, eval_cnt_bound);
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

void move_ordering(const Othello node, const color player, square* moves_arr, int num_moves)
{
    struct order {
        int weight;
        int perm;
    } orders[64];
    square moves_tmp[64];

    for (int i = 0; i < num_moves; i++)
        moves_tmp[i] = moves_arr[i];

    for (int i = 0; i < num_moves; i++) {
        Othello child = node;
        child.make_move(player, moves_arr[i]);

        orders[i].perm = i;
        orders[i].weight = alphabeta(
            child, player, nullptr,
            2, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true,
            mobility_eval, player
        );
        // orders[i].weight = simple_eval(child, player);
    }

    std::sort(orders, orders + num_moves, [](const order& lfs, const order& rhs) {
        return lfs.weight > rhs.weight;
    });
    for (int i = 0; i < num_moves; i++)
        moves_arr[i] = moves_tmp[orders[i].perm];
}