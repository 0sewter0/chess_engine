#include "bitboard.h"
#include "move.h"
#include "move_init.h"

int is_square_attacked(int square, int side, Board *board) {
    if(pawn_attacks[!side][square] & board->bitboards[(side == WHITE) ? P : p]) return 1;

    if(knight_attacks[square] & board->bitboards[(side == WHITE) ? N : n]) return 1;

    if(king_attacks[square] & board->bitboards[(side == WHITE) ? K : k]) return 1;

    if(get_bishop_attacks(square, board->occupancies[BOTH]) & (board->bitboards[(side == WHITE) ? B : b] | board->bitboards[(side == WHITE) ? Q : q])) return 1;

    if(get_rook_attacks(square, board->occupancies[BOTH] & board->bitboards[(side == WHITE) ? R : r] | board->bitboards[(side = WHITE) ? Q : q])) return 1;

    return 0;
}

void generate_moves_knight(Board *board, moves *move_list) {
    int side = board->side;
    int piece = (side == WHITE) ? K : k;
    int source_square = get_lsb_index(board->bitboards[piece]);\

    Bitboard attacks = king_attacks[source_square] & ~board->occupancies[side];
    while(attacks) {
        int target_square = get_lsb_index(attacks);
        if(GET_BIT(board->occupancies[!side], target_square)) {
            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
        } else {
            add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
        }
        CLEAR_BIT(attacks, target_square);
    }

    if(side == WHITE) {
        if(board->castle & WK) {
            if(!GET_BIT(board->occupancies[BOTH], f1) && !GET_BIT(board->occupancies[BOTH], g1)) {
                if(!is_square_attacked(e1, BLACK, board) && !is_square_attacked(f1, BLACK, board) && !is_square_attacked(g1, BLACK, board)) {
                    add_move(move_list, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
        }
    }
}

void generate_moves_king(Board *board, moves *move_list) {
    int side = board->side;
    int piece = (side == WHITE) ? K : k;
    int source_square = get_lsb_index(board->bitboards[piece]);
    Bitboard attacks = king_attacks[source_square] & ~board->occupancies[side];
    while (attacks) {
        int target_square = get_lsb_index(attacks);
        if (GET_BIT(board->occupancies[!side], target_square)) {
            add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
        } else {
            add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
        }
        CLEAR_BIT(attacks, target_square);
    }
    if (side == WHITE) {
        if (board->castle & WK) {
            if (!GET_BIT(board->occupancies[BOTH], f1) && !GET_BIT(board->occupancies[BOTH], g1)) {
                if (!is_square_attacked(e1, BLACK, board) && 
                    !is_square_attacked(f1, BLACK, board) && 
                    !is_square_attacked(g1, BLACK, board)) {
                    
                    add_move(move_list, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
       }
        if (board->castle & WQ) {
            if (!GET_BIT(board->occupancies[BOTH], b1) && 
                !GET_BIT(board->occupancies[BOTH], c1) && 
                !GET_BIT(board->occupancies[BOTH], d1)) {
                
                if (!is_square_attacked(e1, BLACK, board) && 
                    !is_square_attacked(d1, BLACK, board) && 
                    !is_square_attacked(c1, BLACK, board)) {
                    
                    add_move(move_list, encode_move(e1, c1, K, 0, 0, 0, 0, 1));
                }
            }
        }
    } else {
        if (board->castle & BK) {
            if (!GET_BIT(board->occupancies[BOTH], f8) && !GET_BIT(board->occupancies[BOTH], g8)) {
                if (!is_square_attacked(e8, WHITE, board) && 
                    !is_square_attacked(f8, WHITE, board) && 
                    !is_square_attacked(g8, WHITE, board)) {
                    
                    add_move(move_list, encode_move(e8, g8, k, 0, 0, 0, 0, 1));
                }
            }
        }
        if (board->castle & BQ) {
            if (!GET_BIT(board->occupancies[BOTH], b8) && 
                !GET_BIT(board->occupancies[BOTH], c8) && 
                !GET_BIT(board->occupancies[BOTH], d8)) {
                
                if (!is_square_attacked(e8, WHITE, board) && 
                    !is_square_attacked(d8, WHITE, board) && 
                    !is_square_attacked(c8, WHITE, board)) {
                    
                    add_move(move_list, encode_move(e8, c8, k, 0, 0, 0, 0, 1));
                }
            }
        }
    }
}

void generate_moves_pawn(Board *board, moves *move_list) {
    int source_square, target_square;

    Bitboard bitboard, attacks;
    if(board->side == WHITE) {
        bitboard = board->bitboards[P];

        while(bitboard) {
            source_square = get_lsb_index(bitboard);
            target_square = source_square + 8;
            if(target_square >= 0 && !GET_BIT(board->occupancies[BOTH], target_square)) {
                if(source_square >= a7 && source_square <= h7) {
                    add_move(move_list, encode_move(source_square, target_square, P, Q, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, R, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, B, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, N, 0, 0, 0, 0));
                } else {
                    add_move(move_list, encode_move(source_square, target_square, P, 0, 0, 0, 0, 0));

                    if((source_square >= a2 && source_square <= h2) && !GET_BIT(board->occupancies[BOTH], source_square + 16)) {
                        add_move(move_list, encode_move(source_square, source_square + 16, P, 0, 0, 1, 0, 0));
                    }
                }
            }
            attacks = pawn_attacks[WHITE][source_square] & board->occupancies[BLACK];
            while(attacks) {
                target_square = get_lsb_index(attacks);

                if(source_square >= a7 && source_square <= h7) {
                    add_move(move_list, encode_move(source_square, target_square, P, Q, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, R, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, B, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, N, 1, 0, 0, 0));
                } else {
                    add_move(move_list, encode_move(source_square, target_square, P, 0, 1, 0, 0, 0));
                }
                CLEAR_BIT(attacks, target_square);
            }
            if(board->enpassant != no_sq) {
                Bitboard enpassant_attacks = pawn_attacks[WHITE][source_square] & (1ULL << board->enpassant);
                if(enpassant_attacks) {
                    int target_enpassant = get_lsb_index(enpassant_attacks);
                    add_move(move_list, encode_move(source_square, target_enpassant, P, 0, 1, 0, 1, 0));
                }
            }
            CLEAR_BIT(bitboard, source_square);
        }
    } else {
        bitboard = board->bitboards[p];

        while(bitboard) {
            source_square = get_lsb_index(bitboard);
            target_square = source_square - 8;
            if(target_square >= 0 && !GET_BIT(board->occupancies[BOTH], target_square)) {
                if(source_square >= a2 && source_square <= h2) {
                    add_move(move_list, encode_move(source_square, target_square, p, q, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, r, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, b, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, n, 0, 0, 0, 0));
                } else {
                    add_move(move_list, encode_move(source_square, target_square, p, 0, 0, 0, 0, 0));

                    if((source_square >= a7 && source_square <= h7) && !GET_BIT(board->occupancies[BOTH], source_square - 16)) {
                        add_move(move_list, encode_move(source_square, source_square - 16, p, 0, 0, 1, 0, 0));
                    }
                }
            }
            attacks = pawn_attacks[BLACK][source_square] & board->occupancies[WHITE];
            while(attacks) {
                target_square = get_lsb_index(attacks);

                if(source_square >= a2 && source_square <= h2) {
                    add_move(move_list, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, r, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, b, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, n, 1, 0, 0, 0));
                } else {
                    add_move(move_list, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
                }
                CLEAR_BIT(attacks, target_square);
            }
            if(board->enpassant != no_sq) {
                Bitboard enpassant_attacks = pawn_attacks[BLACK][source_square] & (1ULL << board->enpassant);
                if(enpassant_attacks) {
                    int target_enpassant = get_lsb_index(enpassant_attacks);
                    add_move(move_list, encode_move(source_square, target_enpassant, p, 0, 1, 0, 1, 0));
                }
            }
            CLEAR_BIT(bitboard, source_square);
        }
    }
}

void generate_moves_bishop(Board *board, moves *move_list) {
    int side = board->side;
    int piece = (side == WHITE) ? B : b;
    Bitboard bitboard = board->bitboards[piece];

    while(bitboard) {
        int source_square = get_lsb_index(bitboard);
        Bitboard attacks = get_bishop_attacks(source_square, board->occupancies[BOTH]) & ~board->occupancies[side];

        while(attacks) {
            int target_square = get_lsb_index(attacks);

            if(GET_BIT(board->occupancies[!side], target_square)) {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
            } else {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
            }
            CLEAR_BIT(attacks, target_square);
        }
        CLEAR_BIT(bitboard, source_square);
    }
}

void generate_moves_rook(Board *board, moves *move_list) {
    int side = board->side;
    int piece = (side == WHITE) ? R : r;
    Bitboard bitboard = board->bitboards[piece];

    while(bitboard) {
        int source_square = get_lsb_index(bitboard);
        Bitboard attacks = get_rook_attacks(source_square, board->occupancies[BOTH]) & ~board->occupancies[side];

        while(attacks) {
            int target_square = get_lsb_index(attacks);

            if(GET_BIT(board->occupancies[!side], target_square)) {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
            } else {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
            }
            CLEAR_BIT(attacks, target_square);
        }
        CLEAR_BIT(bitboard, source_square);
    }
}

void generate_moves_queen(Board *board, moves *move_list) {
    int side = board->side;
    int piece = (side == WHITE) ? Q : q;
    Bitboard bitboard = board->bitboards[piece];

    while(bitboard) {
        int source_square = get_lsb_index(bitboard);
        Bitboard attacks = get_queen_attacks(source_square, board->occupancies[BOTH]) & ~board->occupancies[side];

        while(attacks) {
            int target_square = get_lsb_index(attacks);

            if(GET_BIT(board->occupancies[!side], target_square)) {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 1, 0, 0, 0));
            } else {
                add_move(move_list, encode_move(source_square, target_square, piece, 0, 0, 0, 0, 0));
            }
            CLEAR_BIT(attacks, target_square);
        }
        CLEAR_BIT(bitboard, source_square);
    }
}

void generate_moves(Board *board, moves *move_list) {
    move_list->count = 0;
    generate_moves_pawn(board, move_list);
    generate_moves_knight(board, move_list);
    generate_moves_king(board, move_list);
    generate_moves_bishop(board, move_list);
    generate_moves_rook(board, move_list);
    generate_moves_queen(board, move_list);
}