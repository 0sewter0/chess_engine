#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdio.h>
#include <stdint.h>

typedef uint64_t Bitboard;

enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
    no_sq
};

enum { P, N, B, R, Q, K, p, n, b, r, q, k};
enum { WHITE, BLACK, BOTH };

typedef struct {
    Bitboard bitboards[12];
    Bitboard occupancies[3];

    int side;
    int enpassant;
    int castle;

    uint64_t hash_key;
    uint64_t hash_history[1024];
    int history_ply;
} Board;

enum { WK = 1, WQ = 2, BK = 4, BQ = 8};



#define SET_BIT(bb, square)  ((bb) |= (1ULL << square))
#define GET_BIT(bb, square)  (((bb) >> (square)) & 1ULL)
#define CLEAR_BIT(bb, square)  ((bb) &= ~(1ULL << (square)))

static inline int get_lsb_index(Bitboard bb) {
    if(bb) {
        return __builtin_ctzll(bb);
    }
    return -1;
}

void print_bitboard(Bitboard bb);
void init_board(Board *board);
void print_board(const Board *board);
void parse_fen(const char *fen, Board *board);

#endif