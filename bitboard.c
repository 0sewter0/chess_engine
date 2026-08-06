#include "bitboard.h"
#include "move.h"
#include "move_init.h"
#include "Zobrist_hash.h"
#include <ctype.h>
#include <string.h>

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

const char piece_ascii[] = "PNBRQKpnbrqk";

uint64_t piece_keys[12][64];
uint64_t enpassant_keys[64];
uint64_t castle_keys[16];
uint64_t side_key;

void init_board(Board *board) {
    parse_fen(START_FEN, board);
    init_all();
}

void print_board(const Board *board) {
    printf("\n");
    for(int rank = 7; rank >= 0; rank--) {
        printf(" %d  ", rank + 1);
        for(int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            int piece = -1;

            for(int bb_piece = P; bb_piece <= k; bb_piece++) {
                if(GET_BIT(board->bitboards[bb_piece], square)) {
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

void parse_fen(const char *fen, Board *board) {
    memset(board, 0, sizeof(Board));
    board->enpassant = no_sq;

    int rank = 7;
    int file = 0;

    while(rank >= 0 && *fen && *fen != ' ') {
        char symbol = *fen;

        if(symbol == '/') {
            rank--;
            file = 0;
        } else if(isdigit(symbol)) {
            int empty_squares = symbol - '0';
            file += empty_squares;
        } else {
            int square = rank * 8 + file;

            switch(symbol) {
                case 'P': SET_BIT(board->bitboards[P], square); break;
                case 'N': SET_BIT(board->bitboards[N], square); break;
                case 'B': SET_BIT(board->bitboards[B], square); break;
                case 'R': SET_BIT(board->bitboards[R], square); break;
                case 'Q': SET_BIT(board->bitboards[Q], square); break;
                case 'K': SET_BIT(board->bitboards[K], square); break;

                case 'p': SET_BIT(board->bitboards[p], square); break;
                case 'n': SET_BIT(board->bitboards[n], square); break;
                case 'b': SET_BIT(board->bitboards[b], square); break;
                case 'r': SET_BIT(board->bitboards[r], square); break;
                case 'q': SET_BIT(board->bitboards[q], square); break;
                case 'k': SET_BIT(board->bitboards[k], square); break;
            }
            file++;
        }
        fen++;
    }
    if(*fen == 'w') {
        board->side = WHITE;
    } else {
        board->side = BLACK;
    }
    fen += 2;

    while(*fen && *fen != ' ') {
        switch(*fen) {
            case 'K': board->castle |= WK; break;
            case 'Q': board->castle |= WQ; break;
            case 'k': board->castle |= BK; break;
            case 'q': board->castle |= BQ; break;
            case '-': break;
        }
        fen++;
    }
    if(*fen == ' ') fen++;

    if(*fen != '-') {
        int ep_file = fen[0] - 'a';
        int ep_rank = fen[1] - '1';
        board->enpassant = ep_rank * 8 + ep_file;
    } else {
        board->enpassant = no_sq;
    }

    update_occupancies(board);
}