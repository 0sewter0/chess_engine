#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H
#include "bitboard.h"

struct moves;

#include "move.h"

int is_square_attacked(int square, int side, Board *board);
int is_in_check(Board *board);
void generate_moves_knight(Board *board, moves *move_list);
void generate_moves_king(Board *board, moves *move_list);
void generate_moves_pawn(Board *board, moves *move_list);
void generate_moves_bishop(Board *board, moves *move_list);
void generate_moves_rook(Board* board, moves *move_list);
void generate_moves_queen(Board *board, moves *move_list);
void generate_moves(Board *board, moves *move_list);

#endif