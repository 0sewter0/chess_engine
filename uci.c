#include "bitboard.h"
#include "search.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "move_generator.h"

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

void parse_position(Board *board, char *command) {
    command += 9;

    if(strncmp(command, "startpos", 8) == 0) {
        board->history_ply = 0;
        parse_fen(START_POS, board);
    } else if(strncmp(command, "fen", 3) == 0) {
        command += 4;
        parse_fen(command, board);
    }

    char *moves_ptr = strstr(command, "moves ");
    if(moves_ptr != NULL) {
        moves_ptr += 6;

        char *token = strtok(moves_ptr, " \n");
        while(token != NULL) {
            uint32_t move = parse_move(token, board);
            if(move != 0) {
                Undo undo;
                make_move(board, move, &undo);
            }
            token = strtok(NULL, " \n");
        }
    }
}

void parse_go(Board *board, char *command) {
    int depth = 6;

    char *depth_ptr = strstr(command, "depth");\
    if(depth_ptr != NULL) {
        depth = atoi(depth_ptr + 6);
    }

    uint32_t best_move = search_position(board, depth);

    printf("bestmove ");
    print_move(best_move);
    printf("\n");
    fflush(stdout);
}

void uci_loop() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    Board board;
    init_board(&board);
    
    char line[4096];

    while(1) {
        if(!fgets(line, sizeof(line), stdin)) break;

        if(strncmp(line, "uci", 3) == 0) {
            printf("id name SRZEngine\n");
            printf("id author Sewter\n");
            printf("uciok\n");
        }
        else if(strncmp(line, "isready", 7) == 0) {
            printf("readyok\n");
        }
        else if(strncmp(line, "ucinewgame", 10) == 0) {
            init_board(&board);
            clear_tt();
        }
        else if(strncmp(line, "position", 8) == 0) {
            parse_position(&board, line);
        }
        else if(strncmp(line, "go", 2) == 0) {
            parse_go(&board, line);
        }
        else if(strncmp(line, "quit", 4) == 0) {
            break;
        }
        fflush(stdout);
    }
}