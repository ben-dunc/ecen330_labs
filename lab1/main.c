/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

// Print out "hello world" on both the console and the LCD screen.

#include "display.h"
#include <stdio.h>

#define TEXT_SIZE 2
#define CURSOR_X 0
#define CURSOR_Y 0

// Center of screen
#define CENTER_HOR DISPLAY_WIDTH / 2
#define CENTER_VER DISPLAY_HEIGHT / 2
#define SHAPE_WIDTH                                                            \
  DISPLAY_WIDTH / 5 // All of the shape coordinates are based on this value.
#define HALF_SHAPE_WIDTH SHAPE_WIDTH / 2

int main() {
  display_init(); // Must init all of the software and underlying hardware for
                  // LCD.
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_setCursor(CURSOR_X, CURSOR_Y); // The upper left of the LCD screen.
  display_setTextColor(DISPLAY_RED);     // Make the text red.
  display_setTextSize(TEXT_SIZE);        // Make the text a little larger.

  // draw lines
  display_drawLine(CURSOR_X, CURSOR_Y, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);
  display_drawLine(DISPLAY_WIDTH, CURSOR_Y, CURSOR_X, DISPLAY_HEIGHT,
                   DISPLAY_GREEN);

  // draw and fill triangles
  display_fillTriangle(CENTER_HOR - HALF_SHAPE_WIDTH, HALF_SHAPE_WIDTH,
                       CENTER_HOR + HALF_SHAPE_WIDTH, HALF_SHAPE_WIDTH,
                       CENTER_HOR, CENTER_VER - HALF_SHAPE_WIDTH,
                       DISPLAY_YELLOW);
  display_drawTriangle(
      CENTER_HOR - HALF_SHAPE_WIDTH, DISPLAY_HEIGHT - HALF_SHAPE_WIDTH,
      CENTER_HOR + HALF_SHAPE_WIDTH, DISPLAY_HEIGHT - HALF_SHAPE_WIDTH,
      CENTER_HOR, CENTER_VER + HALF_SHAPE_WIDTH, DISPLAY_YELLOW);

  // draw and fill circles
  display_drawCircle(CENTER_HOR - SHAPE_WIDTH - HALF_SHAPE_WIDTH, CENTER_VER,
                     HALF_SHAPE_WIDTH, DISPLAY_RED);
  display_fillCircle(CENTER_HOR + SHAPE_WIDTH + HALF_SHAPE_WIDTH, CENTER_VER,
                     HALF_SHAPE_WIDTH, DISPLAY_RED);

  // string to the LCD.
  printf("hello world!"); // This prints on the console.
  return 0;
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in
// supportFiles).
void isr_function() {
  // Empty for now.
}