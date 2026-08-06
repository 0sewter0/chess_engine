#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H
#include <stdint.h>
#include "bitboard.h"

extern uint64_t piece_keys[12][64];

extern uint64_t enpassant_keys[64];

extern uint64_t castle_keys[16];

extern uint64_t side_key;

static inline void init_zobrist() {
    uint64_t seed = 1070372;
    #define C_RAND64() (seed = seed * 6364136223846793005ULL + 1442695040888963407ULL)
    for(int piece = 0; piece < 12; piece++) {
        for(int square = 0; square < 64; square++) {
            piece_keys[piece][square] = C_RAND64();
        }
    }
    for(int square = 0; square < 64; square++) {
        enpassant_keys[square] = C_RAND64();
    }
    for(int i = 0; i < 16; i++) {
        castle_keys[i] = C_RAND64();
    }
    side_key = C_RAND64();
}

static inline uint64_t generate_hash_key(const Board *board) {
    uint64_t final_key = 0ULL;

    for(int piece = P; piece <= K; piece++) {
        Bitboard bitboard = board->bitboards[piece];
        while(bitboard) {
            int square = get_lsb_index(bitboard);
            final_key ^= piece_keys[piece][square];
            CLEAR_BIT(bitboard, square);
        }
    }
    if(board->enpassant != no_sq) {
        final_key ^= enpassant_keys[board->enpassant];
    } 
    final_key ^= castle_keys[board->castle];

    if(board->side == BLACK) {
        final_key ^= side_key;
    }

    return final_key;
}
#endif