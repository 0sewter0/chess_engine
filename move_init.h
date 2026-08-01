#ifndef GENERATE_MOVES_H
#define GENERATE_MOVES_H

#include "bitboard.h"

extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];
extern Bitboard pawn_attacks[2][64];

extern Bitboard rook_attacks_table[64][4096];
extern Bitboard bishop_attacks_table[64][512];

extern Bitboard rook_masks[64];
extern Bitboard bishop_masks[64];

extern const int rook_relevant_bits[64];
extern const int bishop_relevant_bits[64];


void init_knight_attacks(void);
void init_king_attacks(void);
void init_pawn_attacks(void);

Bitboard rook_attacks(int square, Bitboard occupancy);
Bitboard bishop_attacks(int square, Bitboard occupancy);
Bitboard queen_attacks(int square, Bitboard occupancy);
Bitboard mask_rook_attacks(int square);
Bitboard mask_bishop_attacks(int square);
Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask);

int count_bits(Bitboard b);
void init_masks(void);

int get_bishop_magic_index(int sq, Bitboard occupancy);
int get_rook_magic_index(int sq, Bitboard occupancy);

Bitboard get_bishop_attacks(int sq, Bitboard occ);
Bitboard get_rook_attacks(int sq, Bitboard occ);
Bitboard get_queen_attacks(int sq, Bitboard occ);

void init_slider_attacks(void);

#endif