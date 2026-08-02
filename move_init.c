#include "bitboard.h"
#include "magic_numbers.h"
#include "move_init.h"
#include "move.h"

Bitboard knight_attacks[64];

void init_knight_attacks() {
    const Bitboard not_a_file = 0xfefefefefefefefeULL;
    const Bitboard not_h_file = 0x7f7f7f7f7f7f7f7fULL;
    const Bitboard not_ab_file = 0xfcfcfcfcfcfcfcfcULL;
    const Bitboard not_gh_file = 0x3f3f3f3f3f3f3f3fULL;

    for(int i = 0; i < 64; i++) {
        knight_attacks[i] = 0;
        Bitboard knight = 1ULL << i;
        knight_attacks[i] |= (knight & not_h_file) << 17;
        knight_attacks[i] |= (knight & not_a_file) << 15;
        knight_attacks[i] |= (knight & not_gh_file) << 10;
        knight_attacks[i] |= (knight & not_ab_file) << 6;

        knight_attacks[i] |= (knight & not_a_file) >> 17;
        knight_attacks[i] |= (knight & not_h_file) >> 15;
        knight_attacks[i] |= (knight & not_ab_file) >> 10;
        knight_attacks[i] |= (knight & not_gh_file) >> 6;
    }
}

Bitboard king_attacks[64];

void init_king_attacks() {
    const Bitboard not_a_file = 0xfefefefefefefefeULL;
    const Bitboard not_h_file = 0x7f7f7f7f7f7f7f7fULL;

    for(int i = 0; i < 64; i++) {
        king_attacks[i] = 0;
        Bitboard king = 1ULL << i;
        king_attacks[i] |= (king & not_h_file) << 1;
        king_attacks[i] |= (king & not_a_file) >> 1;
        king_attacks[i] |= (king & not_h_file) << 9;
        king_attacks[i] |= (king & not_a_file) << 7;
        king_attacks[i] |= (king & not_h_file) >> 7;
        king_attacks[i] |= (king & not_a_file) >> 9;
        king_attacks[i] |= king << 8;
        king_attacks[i] |= king >> 8;
    }
}

Bitboard pawn_attacks[2][64];

void init_pawn_attacks() {
    const Bitboard not_a_file = 0xfefefefefefefefeULL;
    const Bitboard not_h_file = 0x7f7f7f7f7f7f7f7fULL;

    for(int i = 0; i < 64; i++) {
        pawn_attacks[0][i] = 0;
        pawn_attacks[1][i] = 0;
        Bitboard pawn = 1ULL << i;

        pawn_attacks[0][i] |= (pawn & not_h_file) << 9;
        pawn_attacks[0][i] |= (pawn & not_a_file) << 7;

        pawn_attacks[1][i] |= (pawn & not_h_file) >> 7;
        pawn_attacks[1][i] |= (pawn & not_a_file) >> 9;
    }
}

Bitboard get_rook_attacks(int sq, Bitboard occ) {
    return rook_attacks_table[sq][get_rook_magic_index(sq, occ)];
}

Bitboard get_bishop_attacks(int sq, Bitboard occ) {
    return bishop_attacks_table[sq][get_bishop_magic_index(sq, occ)];
}

Bitboard get_queen_attacks(int sq, Bitboard occ) {
    return get_bishop_attacks(sq, occ) | get_rook_attacks(sq, occ);
}

Bitboard queen_attacks(int square, Bitboard occupancy) {
    return rook_attacks(square, occupancy) | bishop_attacks(square, occupancy);
}

Bitboard rook_attacks(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;
    for(int tr = r+1; tr <= 7; tr++) {
        int sq = tr*8+f;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tr = r-1; tr >= 0; tr--) {
        int sq = tr*8+f;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tf = f+1; tf <= 7; tf++) {
        int sq = tf+8*r;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tf = f-1; tf >= 0; tf--) {
        int sq = tf+8*r;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    return attacks;
}

Bitboard bishop_attacks(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int r = square / 8;
    int f = square % 8;

    for(int tr = r+1, tf = f+1; tr <= 7 && tf <= 7; tr++, tf++) {
        int sq = tr*8+tf;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tr = r+1, tf = f-1; tr <= 7 && tf >= 0; tr++, tf--) {
        int sq = tr*8+tf;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tr = r-1, tf = f+1; tr >= 0 && tf <= 7; tr--, tf++) {
        int sq = tr*8+tf;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    for(int tr = r-1, tf = f-1; tr >= 0 && tf >= 0; tr--, tf--) {
        int sq = tr*8+tf;
        attacks |= (1ULL << sq);
        if(occupancy & (1ULL << sq)) break;
    }
    return attacks;
}


Bitboard mask_rook_attacks(int square) {
    Bitboard attacks = 0ULL;

    int rank = square / 8;
    int file = square % 8;

    for(int target_r = rank + 1; target_r < 7; target_r++) {
        int target_sq = target_r * 8 + file;
        attacks |= (1ULL << target_sq);
    }
    for(int target_r = rank - 1; target_r > 0; target_r--) {
        int target_sq = target_r * 8 + file;
        attacks |= (1ULL << target_sq);
    }
    for(int target_f = file + 1; target_f < 7; target_f++) {
        int target_sq = rank * 8 + target_f;
        attacks |= (1ULL << target_sq);
    }
    for(int target_f = file - 1; target_f > 0; target_f--) {
        int target_sq = rank * 8 + target_f;
        attacks |= (1ULL << target_sq);
    }
    return attacks;
}

Bitboard mask_bishop_attacks(int square) {
    Bitboard attacks = 0ULL;

    int rank = square / 8;
    int file = square % 8;
    
    for(int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++) {
        int target_sq = r * 8 + f;
        attacks |= (1ULL << target_sq);
    }
     for(int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--) {
        int target_sq = r * 8 + f;
        attacks |= (1ULL << target_sq);
    }
     for(int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++) {
        int target_sq = r * 8 + f;
        attacks |= (1ULL << target_sq);
    }
     for(int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--) {
        int target_sq = r * 8 + f;
        attacks |= (1ULL << target_sq);
    }
    return attacks;
}

Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask) {
    Bitboard occupancy = 0ULL;

    for(int count = 0; count < bits_in_mask; count++) {
        int square = __builtin_ctzll(attack_mask);

        attack_mask &= attack_mask - 1;

        if(index & (1 << count)) {
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

int count_bits(Bitboard b) {
    return __builtin_popcountll(b);
}

const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

Bitboard bishop_masks[64];
Bitboard rook_masks[64];

void init_masks() {
    for(int sq = 0; sq < 64; sq++) {
        bishop_masks[sq] = mask_bishop_attacks(sq);
        rook_masks[sq] = mask_rook_attacks(sq);
    }
}

int get_bishop_magic_index(int sq, Bitboard occupancy) {
    occupancy &= bishop_masks[sq];
    occupancy *= bishop_magics[sq];
    return occupancy >> (64 - bishop_relevant_bits[sq]);
}

int get_rook_magic_index(int sq, Bitboard occupancy) {
    occupancy &= rook_masks[sq];
    occupancy *= rook_magics[sq];
    return occupancy >> (64 - rook_relevant_bits[sq]);
}

Bitboard rook_attacks_table[64][4096];
Bitboard bishop_attacks_table[64][512];

void init_slider_attacks() {
    init_masks();
    for(int sq = 0; sq < 64; sq++) {
        int bishop_count = 1 << bishop_relevant_bits[sq];
        for(int i = 0; i < bishop_count; i++) {
            Bitboard occ = set_occupancy(i, bishop_relevant_bits[sq], bishop_masks[sq]);
            int index = get_bishop_magic_index(sq, occ);
            bishop_attacks_table[sq][index] = bishop_attacks(sq, occ);
        }
        int rook_count = 1 << rook_relevant_bits[sq];
        for(int i = 0; i < rook_count; i++) {
            Bitboard occ = set_occupancy(i, rook_relevant_bits[sq], rook_masks[sq]);
            int index = get_rook_magic_index(sq, occ);
            rook_attacks_table[sq][index] = rook_attacks(sq, occ);
        }
    }
}

void init_all() {
    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_masks();
    init_slider_attacks();
}