#include "bitboard.h"
#include "move.h"
#include "move_generator.h"
#include "evaluation.h"

#define INF 10000000
#define MATE_BOUND 1000000

static int evaluate_position(Board *board) {
    int eval = evaluation(board);
    return (board->side == WHITE) ? eval : -eval;
}

static int evaluate_mate(int depth) {
    return -MATE_BOUND + (100 - depth);
}

static int is_mate_or_stalemate(Board *board, moves *move_list) {
    if(move_list->count == 0) {
        return is_in_check(board) ? 1 : 2;
    }
    return 0;
}

int alpha_beta(Board *board, int depth, int alpha, int beta) {
    if(depth == 0) return evaluate_position(board);

    moves move_list;
    generate_moves(board, &move_list);

    int terminal = is_mate_or_stalemate(board, &move_list);
    if(terminal == 1) {
        printf("depth=%d, side=%d\n", depth, board->side);
        return evaluate_mate(depth);
    }
    if(terminal == 2) {
        return 0;
    }
    for(int i = 0;  i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        make_move(board, move_list.moves[i], &undo);

        int score = -alpha_beta(board, depth - 1, -beta, -alpha);

        unmake_move(board, move_list.moves[i], &undo);

        if(score >= beta) {
            return beta;
        }
        if(score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

uint32_t search_position(Board *board, int depth) {
    moves move_list;
    generate_moves(board, &move_list);

    uint32_t best_move = 0;
    int best_score = -INF;
    int alpha = -INF;
    int beta = INF;

    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        make_move(board, move_list.moves[i], &undo);

        int score = -alpha_beta(board, depth - 1, -beta, -alpha);

        unmake_move(board, move_list.moves[i], &undo);

        if(score > best_score) {
            best_score = score;
            best_move = move_list.moves[i];
        }
    }
    printf("info depth %d score cp %d\n", depth, best_score);
    return best_move;
}