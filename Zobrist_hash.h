#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H
#include <stdint.h>
#include "bitboard.h"
#include <string.h>

#define HASH_EXACT 0
#define HASH_ALPHA 1
#define HASH_BETA 2

typedef struct {
    uint64_t hash_key;
    int depth;
    int flag;
    int score;
    uint32_t move;
} TT_entry;

#define TT_SIZE (1024 * 1024)  
extern TT_entry TT[TT_SIZE];

static inline void clear_tt() {
    memset(TT, 0, sizeof(TT));
}

static inline void write_tt(Board *board, int depth, int score, int flag, uint32_t best_move) {
    int index = board->hash_key % TT_SIZE;

    TT[index].hash_key = board->hash_key;
    TT[index].depth = depth;
    TT[index].flag = flag;
    TT[index].score = score;
    TT[index].move = best_move;
}

static inline int read_tt(Board *board, int depth, int alpha, int beta, uint32_t *tt_move) {
    if(board->hash_key == 0) {
        return -10000000 - 1;
    }

    int index = board->hash_key % TT_SIZE;
    TT_entry *entry = &TT[index];

    if(entry->hash_key == board->hash_key) {
        if(tt_move) *tt_move = entry->move;

        if(entry->depth >= depth) {
            if(entry->flag == HASH_EXACT) return entry->score;
            if(entry->flag == HASH_ALPHA && entry->score <= alpha) return alpha;
            if(entry->flag == HASH_BETA && entry->score >= beta) return beta;
        }
    }
    return -10000000 - 1;
}

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

    for(int piece = P; piece <= k; piece++) {
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