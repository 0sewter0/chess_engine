#include "bitboard.h"
#include "move_generator.h"
#include "move.h"
#include "move_init.h"
#include "perft.h"
#include <time.h>
#include <stdio.h>

void perft_test(int depth, Board *board) {
    printf("\n=====================================\n");
    printf("  Perft test  (depth = %d)\n", depth);
    printf("\n=====================================\n");

    moves move_list;
    move_list.count = 0;
    generate_moves_knight(board, &move_list);
    printf("%d\n", move_list.count);

    unsigned long long total_nodes = 0;
    clock_t start = clock();

    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        if(!make_move(board, move_list.moves[i], &undo)) {
            continue;
        }

        unsigned long long nodes = perft(depth - 1, board);
        total_nodes += nodes;

        unmake_move(board, move_list.moves[i], &undo);

        printf("Move %d: %llu nodes\n", i+1, nodes);
    }
    clock_t end = clock();
    double spent_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nNodes: %llu\n", total_nodes);
    printf("time spent: %.3f sec\n", spent_time);
    if(spent_time > 0) {
        printf("speed:         %.2f MNPS\n", (total_nodes / 1000000.0) / spent_time);
    }
    printf("==============================================\n\n");
}

int main() {
    printf("initialization moves\n");
    init_all();

    printf("initialization board\n");
    Board board;
 
    init_board(&board);
    int depth = 1;
    printf("perft test\n");
    perft_test(depth, &board);
    printf("Done\n");
    return 0;
}