// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include "verifySequence.h"
#include "buttonHandler.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TIMEOUT_COUNTER_LIMIT 50
#define SHOW_DEBUG false
#define MINUS_ONE - 1

bool vs_enabled;
bool vs_complete;
bool vs_timeOut;
bool vs_inputError;
uint32_t vs_counter;
uint8_t vs_userInput;

static uint16_t index;

// enuming!
enum vs_st_t {
  init_st,
  touch_st,
  verify_st,
  complete_st,
};
static enum vs_st_t curState;

// State machine will run when enabled.
void verifySequence_enable() { vs_enabled = true; }

// This is part of the interlock. You disable the state-machine and then enable
// it again.
void verifySequence_disable() { vs_enabled = false; }

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError() { return vs_timeOut; }

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError() { return vs_inputError; }

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete() { return vs_complete; }

// output the currentState
void verifySequence_printStateString() {
  static enum vs_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {
    // output thingy
    switch (curState) {
    case init_st:
      printf("init_st\n");
      break;
    case touch_st:
      printf("touch_st\n");
      break;
    case verify_st:
      printf("verify_st\n");
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

// Standard init function.
void verifySequence_init() {
  curState = init_st;
  vs_enabled = false;
  vs_complete = false;
  vs_inputError = false;
  vs_timeOut = false;
  index = 0;
}

// Standard tick function.
void verifySequence_tick() {
  // debug if wanted
  if (SHOW_DEBUG) {
    verifySequence_printStateString();
  }

  // transition switch
  switch (curState) {
  case init_st:
    // transition to touch_st if enabled
    if (vs_enabled) {
      curState = touch_st;
      // enable button
      buttonHandler_enable();
      simonDisplay_drawAllButtons();
    }
    break;
  case touch_st:
    // if disabled, init
    if (!vs_enabled) {
      verifySequence_init();
    }

    // if timeout reached, complete and throw flag
    if (vs_counter >= TIMEOUT_COUNTER_LIMIT) {
      vs_timeOut = true;
      vs_complete = true;
      curState = complete_st;
      // if button completed, get values, disable button, and transition
    } else if (buttonHandler_isComplete()) {
      vs_userInput = buttonHandler_getRegionNumber();
      buttonHandler_disable();
      curState = verify_st;
      vs_counter = 0;
    }
    break;
  case verify_st:
    // if disabled, init
    if (!vs_enabled) {
      verifySequence_init();
    }

    // if correct value and more, index++ and advance
    if (vs_userInput == globals_getSequenceValue(index) &&
        index < globals_getSequenceIterationLength() MINUS_ONE) {
      index++;
      curState = touch_st;
      buttonHandler_enable();
      // else if correct value and no more, complete
    } else if (vs_userInput == globals_getSequenceValue(index)) {
      vs_complete = true;
      curState = complete_st;
      // else if incorect value, complete and throw flag
    } else {
      vs_inputError = true;
      vs_complete = true;
      curState = complete_st;
    }
    break;
  case complete_st:
    // if disabled, init
    if (!vs_enabled) {
      verifySequence_init();
    }
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN buttonHandler debug state!\n");
    break;
  }

  // action switch
  switch (curState) {
  case init_st:
    // nothing
    break;
  case touch_st:
    // timeout vs_counter
    vs_counter++;
    break;
  case verify_st:
    // nothing
    break;
  case complete_st:
    // nothing
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN buttonHandler debug state!\n");
    break;
  }
}
