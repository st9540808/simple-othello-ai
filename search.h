#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "othello.h"

int eval(Bitboard self, Bitboard opponent, Bitboard self_moves, Bitboard opp_moves);
square alphabeta(const Othello node, int depth, const color player);
int alphabeta(const Othello node, int depth, int alpha, int beta,
              const color player, bool maximizingPlayer, square* best_move);

#endif