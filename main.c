#include "bitboard.h"
#include "move_generator.h"
#include "move.h"
#include "move_init.h"
#include "search.h"
#include "evaluation.h"
#include "perft.h"
#include <time.h>
#include <stdio.h>

void perft_test(int depth, Board *board) {
    printf("\n=====================================\n");
    printf("  Perft test  (depth = %d)\n", depth);
    printf("\n=====================================\n");

    moves move_list;
    move_list.count = 0;
    generate_moves(board, &move_list);

    uint64_t total_nodes = 0;
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

        print_move(move_list.moves[i]);
        printf(" : %llu nodes\n", nodes);
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
    Board board;
    init_board(&board);

    search_position(&board, 5);
    return 0;
}