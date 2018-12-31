#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"

int eval(Othello game_state);
square alphabeta(const Othello node, int depth, const color player);
int alphabeta(const Othello node, int depth, int alpha, int beta,
              const color player, bool maximizingPlayer, square* best_move);
square rand_generate(const Othello node, const color player);

#endif