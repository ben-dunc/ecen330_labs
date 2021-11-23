#include "minimax.h"
#include <stdio.h>

#define MOVE_PLACEHOLDER 4
#define SCORE_PLACEHOLDER 15

typedef struct {
  minimax_move_t move;
  minimax_score_t score;
} minimax_strategy_t;

uint64_t boardIndex;

// Recursive helper function. This function will call itself, finding out which
// option is the best one available.
minimax_strategy_t minimax(minimax_board_t *board, bool current_player_is_x,
                           uint16_t depthIndex) {
  /*
      Here's my plan:
      1. Check if game is over. I can use the minimax_computeBoardScore
     minimax_isGameOver functions.
      2. Iterate through all of the empty squares and get their score on each
     iteration. Save the best score and the best move through the iterations.
         Take a look at the pseudo code to see a great for-loop example (except
     that they don't save the values).
      3. Return the score & move.
  */

  minimax_strategy_t strategy;
  strategy.score = SCORE_PLACEHOLDER;
  strategy.move.row = MOVE_PLACEHOLDER;
  strategy.move.column = MOVE_PLACEHOLDER;

  // if already gameover
  if (minimax_isGameOver(
          minimax_computeBoardScore(board, current_player_is_x))) {
    strategy.score = minimax_computeBoardScore(board, current_player_is_x);
    return strategy;
  }

  // iterate through rows
  for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // iterate through columns
    for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      // if the square is empty, use minimax recursion
      if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
        board->squares[i][j] =
            current_player_is_x ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        minimax_strategy_t newStrategy;
        boardIndex++;
        newStrategy = minimax(board, !current_player_is_x, depthIndex + 1);

        // if its the X's turn it likes the new strategy
        if (current_player_is_x && (newStrategy.score > strategy.score ||
                                    strategy.score == SCORE_PLACEHOLDER)) {
          strategy.score = newStrategy.score;
          strategy.move.row = i;
          strategy.move.column = j;
          // if its the O's turn it likes the new strategy
        } else if (!current_player_is_x &&
                   (newStrategy.score < strategy.score ||
                    strategy.score == SCORE_PLACEHOLDER)) {
          strategy.score = newStrategy.score;
          strategy.move.row = i;
          strategy.move.column = j;
        }

        board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }

  strategy.score += strategy.score > 0 ? -1 : 1;

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
void minimax_computeNextMove(minimax_board_t *board, bool current_player_is_x,
                             uint8_t *row, uint8_t *column) {
  boardIndex = 0;

  minimax_strategy_t winningStrategy;
  winningStrategy = minimax(board, current_player_is_x, 0); // TODO!!!!
  *row = winningStrategy.move.row;
  *column = winningStrategy.move.column;
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
minimax_score_t minimax_computeBoardScore(minimax_board_t *board,
                                          bool player_is_x) {
  const uint8_t FIRST_SQUARE = 0;
  const uint8_t SECOND_SQUARE = 1;
  const uint8_t THIRD_SQUARE = 2;

  // X Wins
  if (!player_is_x) {
    // x wins with first column
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // x wins with second column
    if (board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // x wins with third column
    if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }

    // x wins with first row
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // x wins with second row
    if (board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // x wins with third row
    if (board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }

    // x wins with left diagonal
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // x wins with right diagonal
    if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_X_SQUARE &&
        board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
  } else {
    // O wins with first column
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // O wins with second column
    if (board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // O wins with third column
    if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }

    // O wins with first row
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[FIRST_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // O wins with second row
    if (board->squares[SECOND_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // O wins with third row
    if (board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }

    // O wins with left diagonal
    if (board->squares[FIRST_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // O wins with right diagonal
    if (board->squares[FIRST_SQUARE][THIRD_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[SECOND_SQUARE][SECOND_SQUARE] == MINIMAX_O_SQUARE &&
        board->squares[THIRD_SQUARE][FIRST_SQUARE] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
  }

  bool isDraw = true;
  // iterate through columns
  for (int8_t i = 0; i < MINIMAX_BOARD_COLUMNS; i++) {
    // iterate through rows
    for (int8_t j = 0; j < MINIMAX_BOARD_ROWS; j++) {
      // check if empty
      if (board->squares[j][i] == MINIMAX_EMPTY_SQUARE) {
        isDraw = false;
      }
    }
  }

  // if empty, return draw
  if (isDraw) {
    return MINIMAX_DRAW_SCORE;
  }

  return MINIMAX_NOT_ENDGAME;
}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t *board) {
  // iterate through rows
  for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // iterate through columns
    for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
}
