#pragma once
#include "bitboard.h"
#include "move.h"

void generate_moves_knight(Board *board, moves *move_list);
void generate_moves_king(Board *board, moves *move_list);
void generate_moves_pawn(Board *board, moves *move_list);
void generate_moves_bishop(Board *board, moves move_list);
void generate_moves_rook(Board* board, moves *move_list);
void generate_moves_queen(Board *board, moves *move_list);
void generate_moves(Board *board, moves move_list);