#ifndef UCI_H
#define UCI_H
#include "bitboard.h"

void parse_position(Board *board, char *command);
void parse_go(Board *board, char *command);
void uci_loop(void);

#endif