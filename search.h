#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "move.h"

int alpha_beta(Board *board, int depth, int alpha, int beta, int ply);
uint32_t search_position(Board *board, int depth);
int score_move(Board *board, uint32_t move, uint32_t tt_move);
int evaluate_position(Board *board);
int qsearch(Board *board, int alpha, int beta);

#endif