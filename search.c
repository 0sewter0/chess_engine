#include "bitboard.h"
#include "move.h"
#include <time.h>
#include "move_generator.h"
#include "evaluation.h"
#include "search.h"

#define INF 10000000
#define MATE_BOUND 1000000

static const int mvv_lva[6][6] = {
    {15, 25, 35, 45, 55, 0},
    {14, 24, 34, 44, 54, 0},
    {13, 23, 33, 43, 53, 0},
    {12, 22, 32, 42, 52, 0},
    {10, 20, 30, 40, 50, 0},
    { 0,  0,  0,  0,  0, 0}
};


int evaluate_position(Board *board) {
    int eval = evaluation(board);
    return (board->side == WHITE) ? eval : -eval;
}

int evaluate_mate(int depth) {
    return -MATE_BOUND + (100 - depth);
}

int is_mate_or_stalemate(Board *board, moves *move_list) {
    if(move_list->count == 0) {
        return is_in_check(board) ? 1 : 2;
    }
    return 0;
}
    void order_moves(Board *board, moves *move_list) {
    for(int i = 1; i < move_list->count; i++) {
        int current = move_list->moves[i];
        int j = i;
        while(j > 0) {
            int prev = move_list->moves[j - 1];
            if(score_move(board, prev) >= score_move(board, current)) {
                break;
            }
            move_list->moves[j] = prev;
            j--;
        }
        move_list->moves[j] = current;
    }
}

int score_move(Board *board, uint32_t move) {
    if(get_move_capture(move)) {
        int attacker = get_move_piece(move);
        int victim = -1;

        for(int piece = P; piece <= k; piece++) {
            if(GET_BIT(board->bitboards[piece], get_move_target(move))) {
                victim = get_move_piece(piece);
                break;
            }
        }

        if(victim >= 0) {
            return 100000 + mvv_lva[attacker][victim];
        }
    }

    return 0;
}

int qsearch(Board *board, int alpha, int beta) {
    int stand_pat = evaluate_position(board);
    if(stand_pat >= beta) {
        return stand_pat;
    }
    if(stand_pat > alpha) {
        alpha = stand_pat;
    }

    moves move_list;
    generate_moves(board, &move_list);
    order_moves(board, &move_list);

    for(int i = 0; i < move_list.count; i++) {
        if(!get_move_capture(move_list.moves[i])) {
            continue;
        }

        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        if(!make_move(board, move_list.moves[i], &undo)) {
            continue;
        }

        int score = -qsearch(board, -beta, -alpha);
        unmake_move(board, move_list.moves[i], &undo);

        if(score > alpha) {
            alpha = score;
            if(alpha >= beta) {
                break;
            }
        }
    }

    return alpha;
}

int alpha_beta(Board *board, int depth, int alpha, int beta) {
    if(depth == 0) return qsearch(board, alpha, beta);
    moves move_list;
    generate_moves(board, &move_list);

    int terminal = is_mate_or_stalemate(board, &move_list);
    if(terminal == 1) {
        return evaluate_mate(depth);
    }
    if(terminal == 2) {
        return 0;
    }

    order_moves(board, &move_list);

    int best_score = -INF;
    int legal_moves = 0;

    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        if(!make_move(board, move_list.moves[i], &undo)) {
            continue;
        }

        legal_moves++;
        int score = -alpha_beta(board, depth - 1, -beta, -alpha);
        unmake_move(board, move_list.moves[i], &undo);

        if(score > best_score) {
            best_score = score;
        }
        if(score > alpha) {
            alpha = score;
        }
        if(alpha >= beta) {
            break;
        }
    }

    if(legal_moves == 0) {
        return evaluate_mate(depth);
    }

    return best_score;
}

uint32_t search_position(Board *board, int depth) {
    clock_t start = clock();
    moves move_list;
    generate_moves(board, &move_list);

    order_moves(board, &move_list);

    uint32_t best_move = 0;
    int best_score = -INF;
    int alpha = -INF;
    int beta = INF;

    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        if(!make_move(board, move_list.moves[i], &undo)) {
            continue;
        }

        int score = -alpha_beta(board, depth - 1, -beta, -alpha);
        unmake_move(board, move_list.moves[i], &undo);

        if(score > best_score) {
            best_score = score;
            best_move = move_list.moves[i];
        }
        if(score > alpha) {
            alpha = score;
        }
    }
    clock_t end = clock();

    printf("Time spent: %.3f\n", (double)(end - start) / CLOCKS_PER_SEC);
    printf("info depth %d score cp %d\n", depth, best_score);
    print_move(best_move);
    return best_move;
}