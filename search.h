#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"

typedef int (*eval_t)(Othello);
typedef int (*eval2_t)(Othello, color);

int weak_eval(Othello game_state);
int weak_eval_for_player(Othello game_state, color eval_player);
int mobility_eval(Othello game_state);
int mobility_eval_for_player(Othello game_state, color eval_player);
int mobility_eval2(Othello game_state);
int mobility_eval2_for_player(Othello game_state, color eval_player);

square alphabeta(const Othello node, const color player, int depth, eval_t eval = &weak_eval);
int alphabeta(const Othello node, const color player, square* best_move, eval_t eval,
              int depth, int alpha, int beta, bool maximizingPlayer);

square negamax(const Othello node, const color player, int depth, eval2_t eval = &weak_eval_for_player);
int negamax(const Othello node, const color player, square* best_move, eval2_t eval,
            int depth, int alpha, int beta);

square rand_generate(const Othello node, const color player);
square rand_generate_smart(const Othello node, const color player);

void move_ordering(const Othello node, const color player, square* moves_arr, int* weights);

#endif