#ifndef ADVANCE_H
#define ADVANCE_H

#include "othello.h"
#include "search.h"

#include <chrono>

template<typename F, typename ...Args>
inline double duration(F&& func, Args&&... args)
{
    auto start = std::chrono::steady_clock::now();
    std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    std::chrono::duration<double> diff = std::chrono::steady_clock::now()-start;
    return diff.count();
}

static constexpr int eval_cnt_bound = 1000000;
static constexpr int init_depth = 6;

square iterative_alphabeta(const Othello node, const color player,
                           int init_depth = init_depth, int eval_cnt_bound = eval_cnt_bound);
square alphabeta_mo(const Othello& node, const color player, int depth,
                    int* eval_cnt, int eval_cnt_bound = eval_cnt_bound,
                    int* best_score = nullptr, eval2_t eval = mobility_eval2);
int alphabeta_mo(const Othello& node, const color player, square* best_move,
                 int depth, int alpha, int beta, bool maximizingPlayer,
                 eval2_t eval, color eval_player, int* eval_cnt, int eval_cnt_bound);

void move_ordering(const Othello node, const color player, square* moves_arr, int num_moves);

#endif