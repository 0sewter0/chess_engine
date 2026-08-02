#ifndef PERFT_H
#define PERFT_H
#include <stdio.h>
#include "move.h"
#include <time.h>

uint64_t perft(int depth, Board *board) {
    if(depth == 0) {
        return 1ULL;
    }
    moves move_list;
    generate_moves(board, &move_list);

    uint64_t nodes = 0;
    
    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        printf("Trying move: %d/%d\n", i+1, move_list.count);

        if(!make_move(board, move_list.moves[i], &undo)) {
            continue;
        }

        nodes += perft(depth - 1, board);

        unmake_move(board, move_list.moves[i], &undo);
    }
    return nodes;
}
#endif