#include "bitboard.h"
#include "move_generator.h"
#include "move.h"
#include <stdio.h>

int main() {
    int move = encode_move(e2, e4, P, 0, 0, 1, 0, 0);
    printf("Move: ");
    print_move(move);
    printf("\nIs double push? %d\n", get_move_double(move));
    printf("Source sq: %d (%s)\n", get_move_source(move), square_to_coordinates[get_move_source(move)]);
    return 0;
}