#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"

typedef int (*eval_t)(Othello);

int weak_eval(Othello game_state);
square alphabeta(const Othello node, int depth, const color player, eval_t eval = &weak_eval);
int alphabeta(const Othello node, const color player, square* best_move, eval_t eval,
              int depth, int alpha, int beta, bool maximizingPlayer);
square rand_generate(const Othello node, const color player);

#endif