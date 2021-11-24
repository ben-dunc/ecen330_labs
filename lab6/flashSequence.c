// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include <stdbool.h>
#include <stdio.h>

#define FLASH_COUNTER_LIMIT 4
#define ERASE_COUNTER_LIMIT 2
#define MINUS_ONE - 1

bool fs_enabled = false;
bool fs_complete = false;
static uint16_t index;

// enuming!
enum fs_st_t {
  init_st,
  flash_st,
  erase_st,
  complete_st,
};
static enum fs_st_t curState;

uint8_t fs_counter;

// Turns on the state machine. Part of the interlock.
void flashSequence_enable() { fs_enabled = true; }

// Turns off the state machine. Part of the interlock.
void flashSequence_disable() { fs_enabled = false; }

// Other state machines can call this to determine if this state machine is
// finished.
bool flashSequence_isComplete() { return fs_complete; }

// Standard init function.
void flashSequence_init() {
  fs_enabled = false;
  fs_complete = false;
  fs_counter = 0;
  index = 0;
  curState = init_st;
}

// output the currentState
void flashSequence_printStateString() {
  static enum fs_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {

    // output thingy
    switch (curState) {
    case init_st:
      printf("init_st\n");
      break;
    case flash_st:
      printf("flash_st\n");
      break;
    case erase_st:
      printf("erase_st\n");
      break;
    case complete_st:
      printf("complete_st\n");
      break;
    default:
      printf("ERROR!!! UNACCEPTABLE STATE IN flashSequence debug state!\n");
      break;
    }
  }
  prevState = curState;
}

// Standard tick function.
void flashSequence_tick() {
  // transition switch
  switch (curState) {
  case init_st:
    // draw square
    if (fs_enabled) {
      curState = flash_st;
      simonDisplay_drawSquare(globals_getSequenceValue(index),
                              SIMON_DISPLAY_DRAW);
    }
    break;
  case flash_st:
    // if disabled, set to init
    if (!fs_enabled) {
      flashSequence_init();
    }

    // once fs_counter is to limit, check if there is any left, ...
    // if so, transition, reset fs_counter, and erase
    if (fs_counter >= FLASH_COUNTER_LIMIT &&
        index < globals_getSequenceIterationLength() MINUS_ONE) {
      fs_counter = 0;
      curState = erase_st;
      simonDisplay_drawSquare(globals_getSequenceValue(index),
                              SIMON_DISPLAY_ERASE);
      index++;
      // if not, go to completed
    } else if (fs_counter >= FLASH_COUNTER_LIMIT) {
      fs_counter = 0;
      curState = complete_st;
      simonDisplay_drawSquare(globals_getSequenceValue(index),
                              SIMON_DISPLAY_ERASE);
      fs_complete = true;
    }
    break;
  case erase_st:
    // if disabled, set to init
    if (!fs_enabled) {
      flashSequence_init();
    }
    // once fs_counter is to limit, draw square, reset fs_counter, and
    // transition
    if (fs_counter >= ERASE_COUNTER_LIMIT) {
      fs_counter = 0;
      curState = flash_st;
      simonDisplay_drawSquare(globals_getSequenceValue(index),
                              SIMON_DISPLAY_DRAW);
    }
    break;
  case complete_st:
    // if disabled, set to init
    // wait for disable
    if (!fs_enabled) {
      flashSequence_init();
    }
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN flashSequence transition state!\n");
    break;
  }

  // action switch
  switch (curState) {
  case init_st:
    // nothing
    break;
  case flash_st:
    // increment fs_counter
    fs_counter++;
    break;
  case erase_st:
    // increment fs_counter
    fs_counter++;
    break;
  case complete_st:
    // nothing
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN flashSequence action state!\n");
    break;
  }
}
