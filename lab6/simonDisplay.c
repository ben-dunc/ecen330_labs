// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include <display.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <utils.h>

#include "simonDisplay.h"

#define BUTTON_SIZE DISPLAY_HEIGHT / 4
#define BUTTON_OFFSET_X (HALF_WIDTH - BUTTON_SIZE) / 2
#define BUTTON_OFFSET_Y (HALF_HEIGHT - BUTTON_SIZE) / 2

#define SQUARE_SIZE DISPLAY_HEIGHT / 2
#define SQUARE_OFFSET_X (HALF_WIDTH - SQUARE_SIZE) / 2

#define ORIGIN 0
#define HALF_WIDTH DISPLAY_WIDTH / 2
#define HALF_HEIGHT DISPLAY_HEIGHT / 2

// computing
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y) {
  return (x < HALF_WIDTH ? SIMON_DISPLAY_REGION_0 : SIMON_DISPLAY_REGION_1) + (y < HALF_HEIGHT ? SIMON_DISPLAY_REGION_0 : SIMON_DISPLAY_REGION_2);
}

// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase) {
  // region 0
  if (regionNumber == SIMON_DISPLAY_REGION_0) {
    display_fillRect(BUTTON_OFFSET_X, BUTTON_OFFSET_Y, BUTTON_SIZE, BUTTON_SIZE,
                     erase ? DISPLAY_BLACK : DISPLAY_RED);
    // region 1
  } else if (regionNumber == SIMON_DISPLAY_REGION_1) {
    display_fillRect(HALF_WIDTH + BUTTON_OFFSET_X, BUTTON_OFFSET_Y, BUTTON_SIZE,
                     BUTTON_SIZE, erase ? DISPLAY_BLACK : DISPLAY_YELLOW);
    // region 2
  } else if (regionNumber == SIMON_DISPLAY_REGION_2) {
    display_fillRect(BUTTON_OFFSET_X, HALF_HEIGHT + BUTTON_OFFSET_Y,
                     BUTTON_SIZE, BUTTON_SIZE,
                     erase ? DISPLAY_BLACK : DISPLAY_BLUE);
    // region 3
  } else if (regionNumber == SIMON_DISPLAY_REGION_3) {
    display_fillRect(HALF_WIDTH + BUTTON_OFFSET_X,
                     HALF_HEIGHT + BUTTON_OFFSET_Y, BUTTON_SIZE, BUTTON_SIZE,
                     erase ? DISPLAY_BLACK : DISPLAY_GREEN);
    // error - unacceptable region number
  } else {
    printf("ERROR!!! UNACCEPTABLE REGION NUM IN simonDisplay_drawButton!\n");
  }
}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, SIMON_DISPLAY_DRAW);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_DRAW);
}

// Convenience function that erases all of the buttons.
void simonDisplay_eraseAllButtons() {
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_0, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_1, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_2, SIMON_DISPLAY_ERASE);
  simonDisplay_drawButton(SIMON_DISPLAY_REGION_3, SIMON_DISPLAY_ERASE);
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to
// "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase) {
  // region 0
  if (regionNo == SIMON_DISPLAY_REGION_0) {
    display_fillRect(SQUARE_OFFSET_X, ORIGIN, SQUARE_SIZE, SQUARE_SIZE,
                     erase ? DISPLAY_BLACK : DISPLAY_RED);
    // region 1
  } else if (regionNo == SIMON_DISPLAY_REGION_1) {
    display_fillRect(HALF_WIDTH + SQUARE_OFFSET_X, ORIGIN, SQUARE_SIZE,
                     SQUARE_SIZE, erase ? DISPLAY_BLACK : DISPLAY_YELLOW);
    // region 2
  } else if (regionNo == SIMON_DISPLAY_REGION_2) {
    display_fillRect(SQUARE_OFFSET_X, HALF_HEIGHT, SQUARE_SIZE, SQUARE_SIZE,
                     erase ? DISPLAY_BLACK : DISPLAY_BLUE);
    // region 3
  } else if (regionNo == SIMON_DISPLAY_REGION_3) {
    display_fillRect(HALF_WIDTH + SQUARE_OFFSET_X, HALF_HEIGHT, SQUARE_SIZE,
                     SQUARE_SIZE, erase ? DISPLAY_BLACK : DISPLAY_GREEN);
    // error
  } else {
    printf("ERROR!!! UNACCEPTABLE REGION NUM IN simonDisplay_drawSquare!\n");
  }
}

// Runs a brief demonstration of how buttons can be pressed and squares lit up
// to implement the user interface of the Simon game. The routine will continue
// to run until the touchCount has been reached, e.g., the user has touched the
// pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks
// until the time has passed. When you implement the game, you CANNOT use this
// function as we discussed in class. Implement the delay using the non-blocking
// state-machine approach discussed in class.
void simonDisplay_runTest(uint16_t touchCount) {}
