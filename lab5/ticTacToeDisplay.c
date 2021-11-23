#include "ticTacToeDisplay.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "switches.h"
#include "utils.h"

#define THIRD_HEIGHT DISPLAY_HEIGHT / 3
#define THIRD_WIDTH DISPLAY_WIDTH / 3
#define ORIGIN 0

#define SHAPE_SIZE THIRD_HEIGHT * 0.7
#define HALF_SHAPE_SIZE SHAPE_SIZE / 2
#define SHAPE_HOR_OFFSET (THIRD_WIDTH - SHAPE_SIZE) / 2
#define SHAPE_VER_OFFSET (THIRD_HEIGHT - SHAPE_SIZE) / 2

#define DRAW false
#define ERASE true

#define TEST_END_MESSAGE "Test Terminated."

// clear squares
void ticTacToeDisplay_clearSquares() {
  // iterate through rows
  for (uint8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) {
    // iterate through columns
    for (uint8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) {
      ticTacToeDisplay_drawX(i, j, true);
      ticTacToeDisplay_drawO(i, j, true);
    }
  }
}

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init() {
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  ticTacToeDisplay_drawBoardLines();
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase) {
  display_drawLine(column * THIRD_WIDTH + SHAPE_HOR_OFFSET,
                   row * THIRD_HEIGHT + SHAPE_VER_OFFSET,
                   column * THIRD_WIDTH + SHAPE_SIZE + SHAPE_HOR_OFFSET,
                   row * THIRD_HEIGHT + SHAPE_SIZE + SHAPE_VER_OFFSET,
                   erase ? DISPLAY_BLACK : DISPLAY_GREEN);

  display_drawLine(column * THIRD_WIDTH + SHAPE_HOR_OFFSET,
                   row * THIRD_HEIGHT + SHAPE_SIZE + SHAPE_VER_OFFSET,
                   column * THIRD_WIDTH + SHAPE_SIZE + SHAPE_HOR_OFFSET,
                   row * THIRD_HEIGHT + SHAPE_VER_OFFSET,
                   erase ? DISPLAY_BLACK : DISPLAY_GREEN);
}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase) {
  display_drawCircle(column * THIRD_WIDTH + SHAPE_HOR_OFFSET + HALF_SHAPE_SIZE,
                     row * THIRD_HEIGHT + SHAPE_VER_OFFSET + HALF_SHAPE_SIZE,
                     HALF_SHAPE_SIZE, erase ? DISPLAY_BLACK : DISPLAY_GREEN);
}

// After a touch has been detected and after the proper delay, this sets the row
// and column arguments according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t *row,
                                                       uint8_t *column) {
  uint16_t x, y;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);

  *column = x / ((uint16_t)THIRD_WIDTH);
  *row = y / ((uint16_t)THIRD_HEIGHT);
}

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will
// paint an X or an O, depending on whether switch 0 (SW0) is slid up (O) or
// down (X). When BTN0 is pushed, the screen is cleared. The test terminates
// when BTN1 is pushed.
void ticTacToeDisplay_runTest() {
  uint8_t row, column;
  bool touched = false;

  buttons_init();
  switches_init();
  display_init();
  ticTacToeDisplay_init();

  // loop till the BTN1 is pressed
  while (!(buttons_read() & BUTTONS_BTN1_MASK)) {
    // draw X or O
    if (display_isTouched() && !touched) {
      touched = true;
      display_clearOldTouchData();
      utils_msDelay(50);
      ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);

      // draws X if SW0 is down, O otherwise
      if (switches_read() & SWITCHES_SW0_MASK) {
        ticTacToeDisplay_drawO(row, column, DRAW);
        // draw 0
      } else {
        ticTacToeDisplay_drawX(row, column, DRAW);
      }
      // set touch = false
    } else if (!display_isTouched() && touched) {
      touched = false;
    }

    // reset display if BTN0 is pressed
    if (buttons_read() & BUTTONS_BTN0_MASK) {
      ticTacToeDisplay_clearSquares();
    }
  }

  // test terminated
  display_fillScreen(DISPLAY_BLACK);
  display_setCursor(ORIGIN, ORIGIN);
  display_setTextSize(2);
  display_setTextColor(DISPLAY_GREEN);
  display_println(TEST_END_MESSAGE);
}

// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines() {
  display_drawLine(ORIGIN, THIRD_HEIGHT, DISPLAY_WIDTH, THIRD_HEIGHT,
                   DISPLAY_GREEN);
  display_drawLine(ORIGIN, THIRD_HEIGHT + THIRD_HEIGHT, DISPLAY_WIDTH,
                   THIRD_HEIGHT + THIRD_HEIGHT, DISPLAY_GREEN);
  display_drawLine(THIRD_WIDTH, ORIGIN, THIRD_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);
  display_drawLine(THIRD_WIDTH + THIRD_WIDTH, ORIGIN, THIRD_WIDTH + THIRD_WIDTH,
                   DISPLAY_HEIGHT, DISPLAY_GREEN);
}
