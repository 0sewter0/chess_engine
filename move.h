#pragma once
#include <stdio.h>
#include "bitboard.h"

int is_square_attacked(int square, int side, Board *board);

/*
0000 0000 0000 0000 0000 0000 0011 1111 -> source
0000 0000 0000 0000 0000 1111 1100 0000 -> target
0000 0000 0000 0000 1111 0000 0000 0000 -> piece
0000 0000 0000 1111 0000 0000 0000 0000 -> promoted
0000 0000 0001 0000 0000 0000 0000 0000 -> capture flag(20th bit)
0000 0000 0010 0000 0000 0000 0000 0000 -> double push flag(21st bit)
0000 0000 0100 0000 0000 0000 0000 0000 -> en passant flag(22nd bit)
0000 0000 1000 0000 0000 0000 0000 0000 -> castling flag(23rd bit)
*/

//coder
#define encode_move(source, target, piece, promoted, capture, double_push, enpassant, castling)  (((source) & 0x3F) | (((target) & 0x3F) << 6) | (((piece) & 0xF) << 12) | (((promoted) & 0xF) << 16) | (((capture) & 0x1) << 20) | (((double_push) & 0x1) << 21) | (((enpassant) & 0x1) << 22) | (((castling) & 0x1) << 23))

#define get_move_source(move) ((move) & 0x3F)
#define get_move_target(move) (((move) >> 6) & 0x3F)
#define get_move_piece(move) (((move) >> 12) & 0xF)
#define get_move_promoted(move) (((move) >> 16) & 0xF)
#define get_move_capture(move) (((move) >> 20) & 0x1)
#define get_move_double(move) (((move) >> 21) & 0x1)
#define get_move_enpassant(move) (((move) >> 22) & 0x1)
#define get_move_castling(move) (((move) >> 23) & 0x1)

typedef struct {
    int moves[256];
    int count;
} moves;

typedef struct {
    int castle;
    int enpassant;
    int captured;
} Undo;

static inline void unmake_move(Board *board, int move, Undo *undo);
static inline void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

static const char promoted_pieces[] = {
    [Q] = 'q', [R] = 'r', [B] = 'b', [N] = 'n',
    [q] = 'q', [r] = 'r', [b] = 'b', [n] = 'n'
};

static const char *square_to_coordinates[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

static inline void print_move(int move) {
    int src = get_move_source(move);
    int tgt = get_move_target(move);
    int prom = get_move_promoted(move);

    if(prom) {
        printf("%s%s%c", square_to_coordinates[src], square_to_coordinates[tgt], promoted_pieces[prom]);
    } else {
        printf("%s%s", square_to_coordinates[src], square_to_coordinates[tgt]);
    }
}

static inline void update_occupancies(Board *board) {
    board->occupancies[WHITE] = 0ULL;
    board->occupancies[BLACK] = 0ULL;
    board->occupancies[BOTH] = 0ULL;

    for(int p = 0; p <= K; p++) {
        board->occupancies[WHITE] |= board->bitboards[p];
    }

    for(int p = 6; p <= k; p++) {
        board->occupancies[BLACK] |= board->bitboards[p];
    }

    board->occupancies[BOTH] = board->occupancies[WHITE] | board->occupancies[BLACK];
}

static inline int make_move(Board *board, int move, Undo *undo) {
    undo->castle = board->castle;
    undo->captured = 0;
    undo->enpassant = board->enpassant;

    board->enpassant = -1;

    int source = get_move_source(move);
    int target = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted = get_move_promoted(move);
    int capture = get_move_capture(move);
    int double_push = get_move_double(move);
    int enpassant = get_move_enpassant(move);
    int castling = get_move_castling(move);

    int side = board->side;

    CLEAR_BIT(board->bitboards[piece], source);
    SET_BIT(board->bitboards[piece], target);

    if(capture) {
        int start_piece = (side == WHITE) ? p : P;
        int end_piece = (side == WHITE) ? k : K;

        for(int bb_piece = start_piece; bb_piece <= end_piece; bb_piece++) {
            if(GET_BIT(board->bitboards[bb_piece], target)) {
                undo->captured = bb_piece;
                CLEAR_BIT(board->bitboards[bb_piece], target);
                break;
            }
        }
    }

    if(promoted) {
        CLEAR_BIT(board->bitboards[(side == WHITE) ? P : p], target);
        SET_BIT(board->bitboards[promoted], target);
    }
    if(enpassant) {
        undo->captured = (side == WHITE) ? p : P;
        int target_pawn_sq = (side == WHITE) ? (target - 8) : (target + 8);
        CLEAR_BIT(board->bitboards[(side == WHITE) ? p : P], target_pawn_sq);
    }

    board->enpassant = -1;

    if(double_push) {
        board->enpassant = (side == WHITE) ? (target - 8) : (target + 8);
    }

    if(castling) {
        switch(target) {
            case g1: CLEAR_BIT(board->bitboards[R], h1); SET_BIT(board->bitboards[R], f1); break;
            case c1: CLEAR_BIT(board->bitboards[R], a1); SET_BIT(board->bitboards[R], d1); break;
            case g8: CLEAR_BIT(board->bitboards[r], h8); SET_BIT(board->bitboards[r], f8); break;
            case c8: CLEAR_BIT(board->bitboards[r], a8); SET_BIT(board->bitboards[r], d8); break;
        }
    }
    static const int castling_rights[64] = {
        13, 15, 15, 15, 12, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        7, 15, 15, 15, 3, 15, 15, 11
    };
    board->castle &= castling_rights[source];
    board->castle &= castling_rights[target];

    board->side ^= 1;
    update_occupancies(board);

    int king_sq = get_lsb_index(board->bitboards[(side == WHITE) ? K : k]);
    if(is_square_attacked(king_sq, board->side, board)) {
        unmake_move(board, move, undo);
        return 0;
    }
    return 1;
}

static inline void unmake_move(Board *board, int move, Undo *undo) {
    board->side ^= 1;
    int side = board->side;

    int source = get_move_source(move);
    int target = get_move_target(move);
    int piece = get_move_piece(move);
    int promoted = get_move_promoted(move);
    int capture = get_move_capture(move);
    int enpassant = get_move_enpassant(move);
    int castling = get_move_castling(move);

    if (promoted) {
        CLEAR_BIT(board->bitboards[promoted], target);
        SET_BIT(board->bitboards[(side == WHITE) ? P : p], source);
    } else {
        CLEAR_BIT(board->bitboards[piece], target);
        SET_BIT(board->bitboards[piece], source);
    }
    if (capture) {
        if (enpassant) {
            int target_pawn_sq = (side == WHITE) ? (target - 8) : (target + 8);
            SET_BIT(board->bitboards[(side == WHITE) ? p : P], target_pawn_sq);
        } else {
            SET_BIT(board->bitboards[undo->captured], target);
        }
    }

    if (castling) {
        switch (target) {
            case g1:
                CLEAR_BIT(board->bitboards[R], f1); SET_BIT(board->bitboards[R], h1); break;
            case c1: 
                CLEAR_BIT(board->bitboards[R], d1); SET_BIT(board->bitboards[R], a1); break;
            case g8: 
                CLEAR_BIT(board->bitboards[r], f8); SET_BIT(board->bitboards[r], h8); break;
            case c8:
                CLEAR_BIT(board->bitboards[r], d8); SET_BIT(board->bitboards[r], a8); break;
        }
    }
    board->castle = undo->castle;
    board->enpassant = undo->enpassant;

    update_occupancies(board);
}
