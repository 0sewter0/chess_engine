#include "bitboard.h"
#include "generate_moves.h"
#include <time.h>
#include <stdio.h>

int main() {
    init_slider_attacks();

    clock_t start = clock();

    Bitboard dummy = 0ULL;
    for(int i = 0; i < 100000000; i++) {
        dummy ^= get_bishop_attacks(d4, 123456789ULL);
    }

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("%f sec", time_spent);
    return 0;
}