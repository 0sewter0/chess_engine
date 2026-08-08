#include "bitboard.h"
#include "move.h"
#include <time.h>
#include "move_generator.h"
#include "evaluation.h"
#include "search.h"

#define INF 10000000
#define NO_HASH_ENTRY 100000
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
    return -MATE_BOUND + depth;
}

int is_mate_or_stalemate(Board *board, moves *move_list, int ply) {
    if(move_list->count == 0) {
        if(is_in_check(board)) {
            return -MATE_BOUND + ply;
        } else {
            return 0;
        }
    }
    return 1000000;
}

int is_repetition(Board *board) {
    for(int i = 0; i < board->history_ply - 1; i++) {
        if(board->hash_history[i] == board->hash_key) {
            return 1;
        }
    }
    return 0;
}

void order_moves(Board *board, moves *move_list, uint32_t tt_move) {
    for(int i = 1; i < move_list->count; i++) {
        uint32_t current = move_list->moves[i];
        int j = i;
        while(j > 0) {
            uint32_t prev = move_list->moves[j - 1];
            if(score_move(board, prev, tt_move) >= score_move(board, current, tt_move)) {
                break;
            }
            move_list->moves[j] = prev;
            j--;
        }
        move_list->moves[j] = current;
    }
}

int score_move(Board *board, uint32_t move, uint32_t tt_move) {
    if(move == tt_move && tt_move != 0) {
        return 1000000;
    }

    if(get_move_capture(move)) {
        int attacker = get_move_piece(move);
        int victim = -1;

        if(get_move_enpassant(move)) {
            victim = (board->side == WHITE) ? p : P;
        } else {
            for(int piece = P; piece <= k; piece++) {
                if(GET_BIT(board->bitboards[piece], get_move_target(move))) {
                    victim = piece;
                    break;
                }
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
    order_moves(board, &move_list, 0);

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

int alpha_beta(Board *board, int depth, int alpha, int beta, int ply) {
    if(ply > 0 && is_repetition(board)) {
        return 0;
    }
    uint32_t tt_move = 0;

    int tt_score = read_tt(board, depth, alpha, beta, &tt_move);

    if(tt_score != -INF - 1) {
        return tt_score;
    }

    if(depth == 0) return qsearch(board, alpha, beta);

    moves move_list;
    generate_moves(board, &move_list);

    int terminal = is_mate_or_stalemate(board, &move_list, ply);
    if(terminal != 1000000) {
        return terminal;
    }

    order_moves(board, &move_list, tt_move);

    int hash_flag = HASH_ALPHA;
    uint32_t best_move = 0;
    int legal_moves = 0;

    for(int i = 0; i < move_list.count; i++) {
        Undo undo;
        undo.castle = board->castle;
        undo.enpassant = board->enpassant;

        if(!make_move(board, move_list.moves[i], &undo)) continue;

        legal_moves++;

        int score = -alpha_beta(board, depth - 1, -beta, -alpha, ply + 1);
        unmake_move(board, move_list.moves[i], &undo);

        if(score >= beta) {
            write_tt(board, depth, score, HASH_BETA, move_list.moves[i]);
            return beta;
        }

        if(score > alpha) {
            hash_flag = HASH_EXACT;
            alpha = score;
            best_move = move_list.moves[i];
        }
    }

    if(legal_moves == 0) {
        return evaluate_mate(depth);
    }

    write_tt(board, depth, alpha, hash_flag, best_move);

    return alpha;
}

uint32_t search_position(Board *board, int depth) {
    clock_t start = clock();

    moves move_list;
    generate_moves(board, &move_list);

    uint32_t tt_move = 0;
    read_tt(board, depth, -INF, INF, &tt_move);

    order_moves(board, &move_list, tt_move);

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

        int score = -alpha_beta(board, depth - 1, -beta, -alpha, 1);
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

    printf("Time spent: %.5f\n", (double)(end - start) / CLOCKS_PER_SEC);
    print_move(best_move);

    return best_move;
}