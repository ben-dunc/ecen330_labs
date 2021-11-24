// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include "simonDisplay.h"
#include <display.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// defines
#define SHOW_DEBUG false

// global variables
bool bh_enabled = false;
bool bh_complete = false;
bool bh_releaseDetected = false;
bool bh_timeoutOccured = false;
uint8_t bh_regionNumber = 0;
// enuming!
enum bh_st_t {
  init_st,
  wait_touch_st,
  wait_values_st,
  wait_release_st,
  complete_st,
};
static enum bh_st_t curState;

// Get the simon region numbers. See the source code for the region numbering
// scheme.
uint8_t buttonHandler_getRegionNumber() { return bh_regionNumber; }

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable() { bh_enabled = true; }

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable() { bh_enabled = false; }

// Standard init function.
void buttonHandler_init() {
  bh_complete = false;
  bh_releaseDetected = false;
  bh_timeoutOccured = false;
}

// The only thing this function does is return a boolean flag set by the
// buttonHandler state machine. To wit: Once enabled, the buttonHandler
// state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area.
// Next, the buttonHandler state-machine waits until the player removes their
// finger. At this point, the state-machine should set a bool flag that
// indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
// If this function does more than return a boolean set by the buttonHandler
// state machine, you are going about this incorrectly.
bool buttonHandler_releaseDetected() { return bh_releaseDetected; }

// Let's you know that the buttonHander is waiting in the interlock state.
bool buttonHandler_isComplete() { return bh_complete; }

// output the currentState
void buttonHandler_printStateString() {
  static enum bh_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {

    // output thingy
    switch (curState) {
      case init_st:
        printf("init_st\n");
        break;
      case wait_touch_st:
        printf("wait_touch_st\n");
        break;
      case wait_values_st:
        printf("wait_values_st\n");
        break;
      case wait_release_st:
        printf("wait_release_st\n");
        break;
      case complete_st:
        printf("complete_st\n");
        break;
      default:
        printf("ERROR!!! UNACCEPTABLE STATE IN buttonHandler debug state!\n");
        break;
    }
  }

  prevState = curState;
}

// Standard tick function.
void buttonHandler_tick() {
  // if we want to debug
  if (SHOW_DEBUG) {
    buttonHandler_printStateString();
  }

  // transition switch
  switch (curState) {
  case init_st:
    // if enabled, go to wait
    if (bh_enabled) {
      curState = wait_touch_st;
    }
    break;
  case wait_touch_st:
    // if disabled, set to init
    if (!bh_enabled) {
      curState = init_st;
      buttonHandler_init();
    }

    // transition to wait_values_st if touched!
    if (display_isTouched()) {
      display_clearOldTouchData();
      curState = wait_values_st;
    }
    break;
  case wait_values_st:
    // if disabled, set to init
    if (!bh_enabled) {
      curState = init_st;
      buttonHandler_init();
    }

    //  use the state to wait and get values
    curState = wait_release_st;
    int16_t x, y;
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z);
    bh_regionNumber = simonDisplay_computeRegionNumber(x, y);
    simonDisplay_drawSquare(bh_regionNumber, SIMON_DISPLAY_DRAW);
    break;
  case wait_release_st:
    // if disabled, set to init
    if (!bh_enabled) {
      curState = init_st;
      buttonHandler_init();
      simonDisplay_drawSquare(bh_regionNumber, SIMON_DISPLAY_ERASE);
      simonDisplay_drawButton(bh_regionNumber, SIMON_DISPLAY_DRAW);
    }

    // if released, go to complete
    if (!display_isTouched()) {
      curState = complete_st;
      bh_complete = true;
      bh_releaseDetected = true;
      simonDisplay_drawSquare(bh_regionNumber, SIMON_DISPLAY_ERASE);
      simonDisplay_drawButton(bh_regionNumber, SIMON_DISPLAY_DRAW);
    }
    break;
  case complete_st:
    // if disabled, set to init
    if (!bh_enabled) {
      curState = init_st;
      buttonHandler_init();
    }
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN buttonHandler transistion state!\n");
    break;
  }

  // action switch
  switch (curState) {
  case init_st:
    // nothing
    break;
  case wait_touch_st:
    // nothing
    break;
  case wait_values_st:
    // nothing
    break;
  case wait_release_st:
    // nothing
    break;
  case complete_st:
    // nothing
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN buttonHandler action state!\n");
    break;
  }
}

// Allows an external controller to notify the buttonHandler that a time-out has
// occurred.
void buttonHandler_timeOutOccurred() { bh_timeoutOccured = true; }