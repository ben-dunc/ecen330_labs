#include "minimax.h"
#include <stdio.h>

#define MOVE_PLACEHOLDER 4
#define SCORE_PLACEHOLDER 15

typedef struct {
    minimax_move_t move;
    minimax_score_t score;
} minimax_strategy_t;

uint64_t boardIndex;

// Recursive helper function. This function will call itself, finding out which option 
// is the best one available. 
minimax_strategy_t minimax(minimax_board_t *board, bool current_player_is_x, uint16_t depthIndex) {
    /*
        Here's my plan:
        1. Check if game is over. I can use the minimax_computeBoardScore minimax_isGameOver functions.
        2. Iterate through all of the empty squares and get their score on each iteration. 
           Save the best score and the best move through the iterations. 
           Take a look at the pseudo code to see a great for-loop example (except that they don't save the values). 
        3. Return the score & move.
    */

    minimax_strategy_t strategy;
    strategy.score = SCORE_PLACEHOLDER;
    strategy.move.row = MOVE_PLACEHOLDER;
    strategy.move.column = MOVE_PLACEHOLDER;

    // printf("\nSTART Depth = %d - Board = %d START\n", depthIndex, boardIndex);
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            if (board->squares[i][j] == MINIMAX_X_SQUARE) {
                // printf("X");
            } else if (board->squares[i][j] == MINIMAX_O_SQUARE) {
                // printf("O");
            } else if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
                // printf("-");
            } else {
                // printf("E");
            }
        }
        // printf("\n");
    }

    if (minimax_isGameOver(minimax_computeBoardScore(board, current_player_is_x))) {
        // printf("=== GAMEOVER ===\n");
        strategy.score = minimax_computeBoardScore(board, current_player_is_x);
        return strategy;
    }

    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
                board->squares[i][j] = current_player_is_x ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

                minimax_strategy_t newStrategy;
                boardIndex++;
                newStrategy = minimax(board, !current_player_is_x, depthIndex + 1);

                // printf("newStrategy at depth %d: score %d, move(r,c) (%d, %d)\n", depthIndex, newStrategy.score, newStrategy.move.row, newStrategy.move.column);

                if (current_player_is_x && (newStrategy.score > strategy.score || strategy.score == SCORE_PLACEHOLDER)) {
                    // printf("Current Player is %c at depth %d\n", current_player_is_x ? 'X' : 'O', depthIndex);
                    // printf("score %d -> %d, move(r,c) (%d -> %d, %d -> %d)\n", strategy.score, newStrategy.score, strategy.move.row, i, strategy.move.column, j);
                    strategy.score = newStrategy.score - 1;
                    strategy.move.row = i;
                    strategy.move.column = j;
                } else if (!current_player_is_x && (newStrategy.score < strategy.score || strategy.score == SCORE_PLACEHOLDER)) {
                    // printf("Current Player is %c at depth %d\n", current_player_is_x ? 'X' : 'O', depthIndex);
                    // printf("score %d -> %d, move(r,c) (%d -> %d, %d -> %d)\n", strategy.score, newStrategy.score, strategy.move.row, i, strategy.move.column, j);
                    strategy.score = newStrategy.score + 1;
                    strategy.move.row = i;
                    strategy.move.column = j;
                }

                board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
            }
        }
    }
    // printf("\nEND Depth = %d - Board = %d END\n", depthIndex, boardIndex);
    // printf("Strategy for %c: score %d, move(r,c) (%d, %d)\n\n", current_player_is_x ? 'X' : 'O', strategy.score, strategy.move.row, strategy.move.column);

    return strategy;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone.
// It computes the row and column of the next
// move based upon: the current board, the player. 
// true means the computer is X.
// false means the computer is O. 
// When called from the controlling state
// machine, you will call this function as follows:
// 1. If the computer is playing as X, you will call this function with
// current_player_is_x = true.
// 2. If the computer is playing as O, you will call this function with
// current_player_is_x = false. 
// minimax_computeNextMove directly passes the
// current_player_is_x argument into the minimax() (helper) function. To assign
// values to the row and column arguments, you must use the following syntax in
// the body of the function: *row = move_row; *column = move_column; (for
// example).
void minimax_computeNextMove(minimax_board_t *board, bool current_player_is_x, uint8_t *row, uint8_t *column) {
    boardIndex = 0;
    // printf("\n\n********************** STARTING NEW ITERATION **********************\n");
    printf("Starting board - Computer is %c\n", current_player_is_x ? 'X' : 'O');

    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            if (board->squares[i][j] == MINIMAX_X_SQUARE) {
                printf("X");
            } else if (board->squares[i][j] == MINIMAX_O_SQUARE) {
                printf("O");
            } else if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
                printf("-");
            } else {
                printf("E");
            }
        }
        printf("\n");
    }

    minimax_strategy_t winningStrategy;
    winningStrategy = minimax(board, current_player_is_x, 0); // TODO!!!!
    *row = winningStrategy.move.row;
    *column = winningStrategy.move.column;

    // printf("WINNING ITERATION\n");
    // printf("Winning strategy = score: %d, move(r,c): %d, %d\n", winningStrategy.score, winningStrategy.move.row, winningStrategy.move.column);

    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            if (board->squares[i][j] == MINIMAX_X_SQUARE) {
                // printf("X");
            } else if (board->squares[i][j] == MINIMAX_O_SQUARE) {
                // printf("O");
            } else if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
                // printf("-");
            } else {
                // printf("E");
            }
        }
        // printf("\n");
    }


    // ** DONE ** 
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
    return score != MINIMAX_NOT_ENDGAME;
}

// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if current_player_is_x == true, the last thing played was an 'X'.
// (2) if current_player_is_x == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(minimax_board_t *board, bool player_is_x) {
    const uint8_t FIRST_SQUARE = 0;
    const uint8_t SECOND_SQUARE = 1;
    const uint8_t THIRD_SQUARE = 2;

    // X Wins
    if (!player_is_x) {
        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on row 0\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on row 1\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on row 2\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        
        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on col 0\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        if (board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on col 1\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        if (board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on col 2\n");
            return MINIMAX_X_WINNING_SCORE;
        }

        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on diagonal\n");
            return MINIMAX_X_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE && board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE) {
            // printf("X wins on diagonal\n");
            return MINIMAX_X_WINNING_SCORE;
        }
    } else {
        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on row 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on row 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on row 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        
        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on col 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        if (board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on col 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        if (board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on col 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }

        if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on diagonal 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
        if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE && board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE && board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE) {
            // printf("O wins on diagonal 0\n");
            return MINIMAX_O_WINNING_SCORE;
        }
    }

    bool isDraw = true;
    for (int8_t i = 0; i < MINIMAX_BOARD_COLUMNS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_ROWS; j++) {
            if (board->squares[j][i] == MINIMAX_EMPTY_SQUARE) {
                isDraw = false;
            }
        }
    }

    if (isDraw) {
        // printf("DRAW\n");
        return MINIMAX_DRAW_SCORE;
    }

    return MINIMAX_NOT_ENDGAME;
}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t *board) {
    printf("\n** INIT BOARD **\n");
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
            board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
        }
    }
}
