#include "bitboard.h"

const char piece_ascii[] = "PNBRQKpnbrqk";

void init_board(Board *board) {
    for(int i = 0; i < 12; i++) board->piece_boards[i] = 0ULL;
    for(int i = 0; i < 3; i++) board->occupancies[i] = 0ULL;

    for(int sq = a2; sq <= h2; sq++) SET_BIT(board->piece_boards[P], sq);
    SET_BIT(board->piece_boards[R], h1); SET_BIT(board->piece_boards[R], a1);
    SET_BIT(board->piece_boards[N], g1); SET_BIT(board->piece_boards[N], b1);
    SET_BIT(board->piece_boards[B], f1); SET_BIT(board->piece_boards[B], c1);
    SET_BIT(board->piece_boards[Q], d1);
    SET_BIT(board->piece_boards[K], e1);

    for(int sq = a7; sq <= h7; sq++) SET_BIT(board->piece_boards[p], sq);
    SET_BIT(board->piece_boards[r], a8); SET_BIT(board->piece_boards[r], h8);
    SET_BIT(board->piece_boards[n], g8); SET_BIT(board->piece_boards[n], b8);
    SET_BIT(board->piece_boards[b], f8); SET_BIT(board->piece_boards[b], c8);
    SET_BIT(board->piece_boards[q], d8);
    SET_BIT(board->piece_boards[k], e8);

    for(int piece = P; piece <= K; piece++) {
        board->occupancies[WHITE] |= board->piece_boards[piece];
    }
    for(int piece = p; piece <= k; piece++) {
        board->occupancies[BLACK] |= board->piece_boards[piece];
    }
    board->occupancies[BOTH] = board->occupancies[WHITE] | board->occupancies[BLACK];
}

void print_board(const Board *board) {
    printf("\n");
    for(int rank = 7; rank >= 0; rank--) {
        printf(" %d  ", rank + 1);
        for(int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1;

            for(int bb_piece = P; bb_piece <= k; bb_piece++) {
                if(GET_BIT(board->piece_boards[bb_piece], square)) {
                    piece = bb_piece;
                    break;
                }
            }
            if(piece != -1) {
                printf(" %c", piece_ascii[piece]);
            } else {
                printf(" .");
            }
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n");
}

void print_bitboard(Bitboard bb) {
    printf("\n");
    for(int rank = 7; rank >= 0; rank--) {
        printf(" %d  ", rank + 1);
        for(int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            printf(" %d", GET_BIT(bb, square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h\n\n");
    printf(" Bitboard Value: %lluULL\n\n", (unsigned long long)bb);
}