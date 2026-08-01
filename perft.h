#pragma once
#include <stdio.h>
#include "move.h"
#include <time.h>

unsigned long long leaf_nodes = 0;

void perft(int depth, Board *board) {
    if(depth == 0) {
        leaf_nodes++;
        return;
    }
    moves move_list;
    move_list.count = 0;

    generate
}