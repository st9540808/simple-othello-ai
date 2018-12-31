#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"

typedef int (*eval_t)(Othello);
typedef int (*eval2_t)(Othello, color);

int weak_eval(Othello game_state);
int weak_eval_for_player(Othello game_state, color eval_player);

square alphabeta(const Othello node, const color player, int depth, eval_t eval = &weak_eval);
int alphabeta(const Othello node, const color player, square* best_move, eval_t eval,
              int depth, int alpha, int beta, bool maximizingPlayer);

square negamax(const Othello node, const color player, int depth, eval2_t eval = &weak_eval_for_player);
int negamax(const Othello node, const color player, square* best_move, eval2_t eval,
            int depth, int alpha, int beta);
square rand_generate(const Othello node, const color player);

#endif