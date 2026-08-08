#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H
#include "move.h"
#include "bitboard.h"

int is_square_attacked(int square, int side, Board *board);
int is_in_check(Board *board);
void generate_moves_knight(Board *board, moves *move_list);
void generate_moves_king(Board *board, moves *move_list);
void generate_moves_pawn(Board *board, moves *move_list);
void generate_moves_bishop(Board *board, moves *move_list);
void generate_moves_rook(Board* board, moves *move_list);
void generate_moves_queen(Board *board, moves *move_list);
void generate_moves(Board *board, moves *move_list);

static inline uint32_t parse_move(const char *input, Board *board) {
    moves move_list;
    generate_moves(board, &move_list);

    for(int i = 0; i < move_list.count; i++) {
        uint32_t move = move_list.moves[i];

        int source = get_move_source(move);
        int target = get_move_target(move);
        int promoted = get_move_promoted(move);

        char generated_str[6];
        if(promoted) {
            snprintf(generated_str, sizeof(generated_str), "%s%s%c", square_to_coordinates[source], square_to_coordinates[target], promoted_pieces[promoted]);
        } else {
            snprintf(generated_str, sizeof(generated_str), "%s%s", square_to_coordinates[source], square_to_coordinates[target]);
        }

        if(strcmp(input, generated_str) == 0) {
            return move;
        }
    }
    return 0;
}

#endif