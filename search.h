#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"


// typedef int (*eval_t)(Othello);
typedef int (*eval2_t)(Othello, color);

int simple_eval(Othello game_state, color eval_player);
int weak_eval_for_player(Othello game_state, color eval_player);
int mobility_eval_for_player(Othello game_state, color eval_player);
int mobility_eval2_for_player(Othello game_state, color eval_player);

square alphabeta(const Othello node, const color player, int depth, eval2_t eval = &weak_eval_for_player);
int alphabeta(const Othello node, const color player, square* best_move,
              int depth, int alpha, int beta, bool maximizingPlayer, eval2_t eval, color eval_player);

square negamax(const Othello node, const color player, int depth, eval2_t eval = &weak_eval_for_player);
int negamax(const Othello node, const color player, square* best_move, eval2_t eval,
            int depth, int alpha, int beta);

square rand_generate(const Othello node, const color player);
square rand_generate_smart(const Othello node, const color player);

// define alias for evaluation functions
extern eval2_t weak_eval;
extern eval2_t mobility_eval;
extern eval2_t mobility_eval2;

#endif