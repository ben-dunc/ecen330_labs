/*
    Created by Benjamin Duncan
*/

#include "clockDisplay.h"
#include <display.h>
#include <stdbool.h>
#include <stdio.h>

#define CHAR_WIDTH DISPLAY_CHAR_WIDTH *CLOCKDISPLAY_TEXT_SIZE
#define CHAR_HEIGHT DISPLAY_CHAR_HEIGHT *CLOCKDISPLAY_TEXT_SIZE
#define HALF_DISPLAY_HEIGHT DISPLAY_HEIGHT / 2
#define HALF_CHAR_HEIGHT CHAR_HEIGHT / 2
#define TIME_STRING_LENGTH 9
#define BLANK_TIME "        "
#define FORCE_DISPLAY true

#define SECS_TENS_PLACE 6
#define MINS_TENS_PLACE 3
#define ZERO_IN_TENS_PLACE_CAP 10
#define ZERO_CHAR '0'

#define SECS_CAP 59
#define MINS_CAP 59
#define HOURS_CAP 12
#define HOURS_START 1

int8_t hours = 12;
int8_t mins = 59;
int8_t secs = 59;

char curTime[TIME_STRING_LENGTH] = "00:00:00";

int16_t clockDisplay_xFromCenter(int16_t x) {
  return (DISPLAY_WIDTH / DISPLAY_BIN) + x;
}

void clockDisplay_checkTimeCaps() {
  if (secs > SECS_CAP) {
    mins++;
    secs = 0;
  }
  if (mins > MINS_CAP) {
    hours++;
    mins = 0;
  }
  if (hours > HOURS_CAP) {
    hours = HOURS_START;
  }

  if (secs < 0) {
    mins--;
    secs = SECS_CAP;
  }
  if (mins < 0) {
    hours--;
    mins = MINS_CAP;
  }
  if (hours < HOURS_START) {
    hours = HOURS_CAP;
  }
}

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll) {

  char newTime[TIME_STRING_LENGTH] = "";
  sprintf(newTime, "%2hd:%2hd:%2hd", hours, mins, secs);

  if (mins < ZERO_IN_TENS_PLACE_CAP) {
    newTime[MINS_TENS_PLACE] = ZERO_CHAR;
  }
  if (secs < ZERO_IN_TENS_PLACE_CAP) {
    newTime[SECS_TENS_PLACE] = ZERO_CHAR;
  }

  if (newTime != curTime || forceUpdateAll) {
    char eraseStr[TIME_STRING_LENGTH] = BLANK_TIME;
    char drawStr[TIME_STRING_LENGTH] = BLANK_TIME;
    for (uint8_t i = 0; i < TIME_STRING_LENGTH; i++) {
      if (newTime[i] != curTime[i] || forceUpdateAll) {
        eraseStr[i] = curTime[i];
        drawStr[i] = newTime[i];
      }
      curTime[i] = newTime[i];
    }

    display_setCursor(clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH -
                                               CHAR_WIDTH - CHAR_WIDTH),
                      HALF_DISPLAY_HEIGHT - HALF_CHAR_HEIGHT);
    display_setTextColor(DISPLAY_BLACK);
    display_print(eraseStr);

    display_setCursor(clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH -
                                               CHAR_WIDTH - CHAR_WIDTH),
                      HALF_DISPLAY_HEIGHT - HALF_CHAR_HEIGHT);
    display_setTextColor(DISPLAY_GREEN);
    display_print(drawStr);
  }
}

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init() {
  // init display
  display_init();
  display_fillScreen(DISPLAY_BLACK);

  // draw text
  display_setTextSize(CLOCKDISPLAY_TEXT_SIZE);
  clockDisplay_updateTimeDisplay(FORCE_DISPLAY);

  // fill triangles
  display_fillTriangle(
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH - CHAR_WIDTH -
                               CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH - CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT - CHAR_WIDTH, DISPLAY_GREEN);

  display_fillTriangle(
      clockDisplay_xFromCenter(-CHAR_WIDTH), HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH), HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(0),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT - CHAR_WIDTH, DISPLAY_GREEN);

  display_fillTriangle(
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH + CHAR_WIDTH +
                               CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH + CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT - CHAR_HEIGHT - CHAR_WIDTH, DISPLAY_GREEN);

  display_fillTriangle(
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH - CHAR_WIDTH -
                               CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(-CHAR_WIDTH - CHAR_WIDTH - CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT + CHAR_WIDTH, DISPLAY_GREEN);

  display_fillTriangle(
      clockDisplay_xFromCenter(-CHAR_WIDTH), HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH), HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(0),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT + CHAR_WIDTH, DISPLAY_GREEN);

  display_fillTriangle(
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH + CHAR_WIDTH +
                               CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT,
      clockDisplay_xFromCenter(CHAR_WIDTH + CHAR_WIDTH + CHAR_WIDTH),
      HALF_DISPLAY_HEIGHT + CHAR_HEIGHT + CHAR_WIDTH, DISPLAY_GREEN);
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec() {
  // Find out where the user touched and inc/dec accordingly.
  int16_t x, y = 0;
  int8_t z = 0;

  display_getTouchedPoint(&x, &y, &z);
  if (x < clockDisplay_xFromCenter(-CHAR_WIDTH - (CHAR_WIDTH / DISPLAY_BIN))) {
    if (y < HALF_DISPLAY_HEIGHT) {
      hours++;
    } else {
      hours--;
    }
  } else if (x < clockDisplay_xFromCenter(CHAR_WIDTH +
                                          (CHAR_WIDTH / DISPLAY_BIN))) {
    if (y < HALF_DISPLAY_HEIGHT) {
      mins++;
    } else {
      mins--;
    }
  } else {
    if (y < HALF_DISPLAY_HEIGHT) {
      secs++;
    } else {
      secs--;
    }
  }

  clockDisplay_checkTimeCaps();
  clockDisplay_updateTimeDisplay(!FORCE_DISPLAY);
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond() {
  secs++;
  clockDisplay_checkTimeCaps();
  clockDisplay_updateTimeDisplay(!FORCE_DISPLAY);
}

// Run a test of clock-display functions.
void clockDisplay_runTest() { clockDisplay_init(); }
