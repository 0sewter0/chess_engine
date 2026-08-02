#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "move.h"

int alpha_beta(Board *board, int depth, int alpha, int beta);
uint32_t search_position(Board *board, int depth);

#endif