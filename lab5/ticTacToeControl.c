#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToeDisplay.h"
#include <stdio.h>

#define TIMER_DONE 60
#define TEXT_SIZE 2
#define CHAR_HEIGHT DISPLAY_CHAR_HEIGHT *TEXT_SIZE

#define STRING_1_X                                                             \
  (DISPLAY_WIDTH / 2) - (DISPLAY_CHAR_WIDTH * TEXT_SIZE * 21 / 2)
#define STRING_1_Y (DISPLAY_HEIGHT / 2) - DISPLAY_CHAR_HEIGHT *TEXT_SIZE * 2
#define STRING_2_X                                                             \
  (DISPLAY_WIDTH / 2) - (DISPLAY_CHAR_WIDTH * TEXT_SIZE * 4 / 2)
#define STRING_2_Y (DISPLAY_HEIGHT / 2) - DISPLAY_CHAR_HEIGHT *TEXT_SIZE
#define STRING_3_X                                                             \
  (DISPLAY_WIDTH / 2) - (DISPLAY_CHAR_WIDTH * TEXT_SIZE * 21 / 2)
#define STRING_3_Y (DISPLAY_HEIGHT / 2)
#define STRING_4_X                                                             \
  (DISPLAY_WIDTH / 2) - (DISPLAY_CHAR_WIDTH * TEXT_SIZE * 11 / 2)
#define STRING_4_Y (DISPLAY_HEIGHT / 2) + DISPLAY_CHAR_HEIGHT *TEXT_SIZE

#define STRING_1 "Touch board to play X"
#define STRING_2 "-or-"
#define STRING_3 "wait for the computer"
#define STRING_4 "and play O"

#define FIRST_SQUARE 0

#define SHOW_DEBUG false

enum ticTacToe_st_t {
  init_st,
  intro_st,
  new_game_st,
  player_turn_st,
  touched_st,
  get_values_st,
  fill_square_st,
  comp_turn_st,
  game_over_st
};
static enum ticTacToe_st_t currentState;

static uint8_t timer3Sec = 0;
static bool playerIsX;
static bool oldTouch = false;
static minimax_board_t board;
static minimax_move_t move;

// intro text
void ticTacToeControl_WriteIntro(bool erase) {
  display_setTextSize(TEXT_SIZE);
  display_setTextColor(erase ? DISPLAY_BLACK : DISPLAY_GREEN);
  display_setCursor(STRING_1_X, STRING_1_Y);
  display_println(STRING_1);
  display_setCursor(STRING_2_X, STRING_2_Y);
  display_println(STRING_2);
  display_setCursor(STRING_3_X, STRING_3_Y);
  display_println(STRING_3);
  display_setCursor(STRING_4_X, STRING_4_Y);
  display_println(STRING_4);
}

// clear squares
void ticTacToeControl_clearSquares() {
  // iterate through rows
  for (uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // iterate through columns
    for (uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      ticTacToeDisplay_drawX(i, j, true);
      ticTacToeDisplay_drawO(i, j, true);
    }
  }
}

// get the string from a string
void ticTacToeControl_printStateString(enum ticTacToe_st_t state) {
  // get string
  switch (state) {
  case init_st:
    printf("init_st");
    break;
  case new_game_st:
    printf("new_game_st");
    break;
  case player_turn_st:
    printf("player_turn_st");
    break;
  case touched_st:
    printf("touched_st");
    break;
  case get_values_st:
    printf("get_values_st");
    break;
  case fill_square_st:
    printf("fill_square_st");
    break;
  case comp_turn_st:
    printf("comp_turn_st");
    break;
  case game_over_st:
    printf("game_over_st");
    break;
  default:
    printf("UNKNOWN");
    break;
  }
}

// debug function
void ticTacToeControl_debug() {
  static enum ticTacToe_st_t pastState = init_st;
  // if state has transitioned
  if (pastState == currentState) {
    return;
  }

  printf("\nDEBUG: ");
  ticTacToeControl_printStateString(pastState);
  printf(" -> ");
  ticTacToeControl_printStateString(currentState);
  printf("\n");

  pastState = currentState;
}

// Tick the tic-tac-toe conroller state machine
void ticTacToeControl_tick() {
  if (SHOW_DEBUG) {
    ticTacToeControl_debug();
  }

  // Perform state update first.
  switch (currentState) {
  case init_st:
    // transition to into and show intro stuff
    ticTacToeControl_WriteIntro(false);
    currentState = intro_st;
    break;
  case intro_st:
    // transition to new_game_st
    if (timer3Sec > TIMER_DONE) {
      currentState = new_game_st;

      // clear squares
      minimax_initBoard(&board);
      // clear intro stuff
      ticTacToeControl_WriteIntro(true);
      // draw grid
      ticTacToeDisplay_drawBoardLines();
      timer3Sec = 0;
    }
    break;
  case new_game_st:
    // after 3 secs
    if (timer3Sec > TIMER_DONE) {
      currentState = player_turn_st;
      playerIsX = false;
      board.squares[FIRST_SQUARE][FIRST_SQUARE] = MINIMAX_X_SQUARE;
      !playerIsX ? ticTacToeDisplay_drawX(FIRST_SQUARE, FIRST_SQUARE, false)
                 : ticTacToeDisplay_drawO(FIRST_SQUARE, FIRST_SQUARE, false);
      timer3Sec = 0;
      // if the player touches it before 3 secs
    }
    if (display_isTouched()) {
      currentState = touched_st;

      playerIsX = true;
    }
    break;
  case player_turn_st:
    // player touches and the old touch is done.
    if (!oldTouch && display_isTouched()) {
      currentState = touched_st;
    }
    // if gameover
    if (minimax_isGameOver(minimax_computeBoardScore(&board, playerIsX))) {
      currentState = game_over_st;
    }
    break;
  case touched_st:
    currentState = get_values_st;
    break;
  case get_values_st:
    // if touch is not in empty square
    if (board.squares[move.row][move.column] != MINIMAX_EMPTY_SQUARE) {
      currentState = player_turn_st;
    }
    // comfirm touch && touch in empty square
    else {
      board.squares[move.row][move.column] =
          playerIsX ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
      currentState = fill_square_st;
    }
    break;
  case fill_square_st:
    // if gameover
    if (minimax_isGameOver(minimax_computeBoardScore(&board, playerIsX))) {
      currentState = game_over_st;
    }
    // else go to comp
    currentState = comp_turn_st;
    break;
  case comp_turn_st:
    currentState = player_turn_st;
    break;
  case game_over_st:
    // if btn0 is pressed, reinit board.
    if (buttons_read() & BUTTONS_BTN0_MASK) {
      currentState = new_game_st;

      // clear squares
      minimax_initBoard(&board);
      // reinit display
      ticTacToeControl_clearSquares();
      timer3Sec = 0;
    }
    break;
  default:
    break;
  }

  // State action switch statement
  switch (currentState) {
  case init_st:
    // do Nothing
    break;
  case intro_st:
    timer3Sec++;
    break;
  case new_game_st:
    timer3Sec++;
    break;
  case player_turn_st:
    // check if old touch is done
    if (oldTouch && !display_isTouched()) {
      oldTouch = false;
    }
    break;
  case touched_st:
    // nothing
    display_clearOldTouchData();
    oldTouch = true;
    break;
  case get_values_st:
    // get the values and assign to move.
    ticTacToeDisplay_touchScreenComputeBoardRowColumn(&move.row, &move.column);
    break;
  case fill_square_st:
    // check if old touch is done
    if (oldTouch && !display_isTouched()) {
      oldTouch = false;
    }
    // fill square
    playerIsX ? ticTacToeDisplay_drawX(move.row, move.column, false)
              : ticTacToeDisplay_drawO(move.row, move.column, false);
    break;
  case comp_turn_st:
    // check if old touch is done
    if (oldTouch && !display_isTouched()) {
      oldTouch = false;
    }
    // do computer turn
    minimax_computeNextMove(&board, !playerIsX, &move.row, &move.column);
    board.squares[move.row][move.column] =
        playerIsX ? MINIMAX_O_SQUARE : MINIMAX_X_SQUARE;
    !playerIsX ? ticTacToeDisplay_drawX(move.row, move.column, false)
               : ticTacToeDisplay_drawO(move.row, move.column, false);
    break;
  case game_over_st:
    // nothing :)
    break;
  default:
    break;
  }
}

// Initialize the tic-tac-toe conroller state machine
void ticTacToeControl_init() {
  display_init();
  buttons_init();

  timer3Sec = 0;
}
