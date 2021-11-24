// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include "display.h"
#include "flashSequence.h"
#include "globals.h"
#include "simonDisplay.h"
#include "verifySequence.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SEQUENCE_LENGTH 4
#define DEFAULT_ITERATION_LENGTH 1
#define GAME_OVER_COUNTER_LIMIT 50
#define SHOW_DEBUG false

#define TITLE_TEXT_SIZE 3
#define TEXT_SIZE 2
#define TITLE_CHAR_SIZE_WIDTH TITLE_TEXT_SIZE *DISPLAY_CHAR_WIDTH
#define TITLE_CHAR_SIZE_HEIGHT TITLE_TEXT_SIZE *DISPLAY_CHAR_HEIGHT
#define CHAR_SIZE_WIDTH TEXT_SIZE *DISPLAY_CHAR_WIDTH
#define CHAR_SIZE_HEIGHT TEXT_SIZE *DISPLAY_CHAR_HEIGHT

#define MAIN_TITLE "SIMON"
#define MAIN_TITLE_LENGTH 2.5
#define MAIN_TEXT_1 "Touch to play"
#define MAIN_TEXT_1_LENGTH 6.5
#define MAIN_TEXT_2 "Length: 4"
#define MAIN_TEXT_2_LENGTH 4.5

#define GAME_OVER_TITLE_WIN "YAY!!! :D"
#define GAME_OVER_TITLE_WIN_LENGTH 4.5
#define GAME_OVER_TITLE_LOSE "SHUCKS. D:<"
#define GAME_OVER_TITLE_LOSE_LENGTH 5.5
#define GAME_OVER_TEXT_1a "Score: "
#define GAME_OVER_TEXT_1a_LENGTH 4
#define GAME_OVER_TEXT_1b "/"
#define GAME_OVER_TEXT_1b_LENGTH 1
#define GAME_OVER_TEXT_2 "Touch to play again! :)"
#define GAME_OVER_TEXT_2_LENGTH 11.5

#define HALF_WIDTH DISPLAY_WIDTH / 2
#define HALF_HEIGHT DISPLAY_HEIGHT / 2
#define HALF 2

bool sc_enabled;
uint32_t sequenceLength;
uint32_t iterationLength;
uint32_t sc_counter;

// enuming!
enum sc_st_t { init_st, main_st, flash_st, verify_st, game_over_st };
static enum sc_st_t curState;

// Enables the control state machine.
void simonControl_enable() { sc_enabled = true; }

// Disables the controll state machine.
void simonControl_disable() { sc_enabled = false; }

// If you want to stop the game after a mistake, check this function after each
// tick.
bool simonControl_isGameOver() { return curState == game_over_st; }

// Use this to set the sequence length. The sequence set by this function
// will only be recognized when the controller passes through its init state.
void simonControl_setSequenceLength(uint16_t length) {
  // get rando seed from time
  int16_t x, y;
  uint8_t z;
  display_getTouchedPoint(&x, &y, &z);

  srand(sc_counter * x * y);

  // use rando seed to fill in thingy
  uint8_t sequence[length];
  // assign
  for (uint16_t i = 0; i < length; i++) {
    sequence[i] = rand() % SIMON_DISPLAY_REGION_COUNT;
  }

  // set it in globals
  globals_setSequence(sequence, length);
  iterationLength = DEFAULT_ITERATION_LENGTH;
  globals_setSequenceIterationLength(iterationLength);
}

// draw main screen
void simonControl_drawMainScreen() {

  // "Simon"
  display_setTextColor(DISPLAY_GREEN);
  display_setTextSize(TITLE_TEXT_SIZE);
  display_setCursor(HALF_WIDTH - MAIN_TITLE_LENGTH * TITLE_CHAR_SIZE_WIDTH,
                    HALF_HEIGHT - TITLE_CHAR_SIZE_HEIGHT -
                        TITLE_CHAR_SIZE_HEIGHT);
  display_print(MAIN_TITLE);

  // "Touch to play"
  display_setTextColor(DISPLAY_WHITE);
  display_setTextSize(TEXT_SIZE);
  display_setCursor(HALF_WIDTH - (MAIN_TEXT_1_LENGTH)*CHAR_SIZE_WIDTH,
                    HALF_HEIGHT);
  display_print(MAIN_TEXT_1);

  // "Length: 4"
  display_setTextColor(DISPLAY_YELLOW);
  display_setCursor(HALF_WIDTH - (MAIN_TEXT_2_LENGTH)*CHAR_SIZE_WIDTH,
                    HALF_HEIGHT + CHAR_SIZE_HEIGHT + CHAR_SIZE_HEIGHT);
  display_print(MAIN_TEXT_2);
}

// draws the game over screen
void simonControl_drawGameOverScreen() {
  bool won =
      !verifySequence_isTimeOutError() && !verifySequence_isUserInputError();

  // if win: "YAY!", if lose: "SHUCKS."
  display_setTextColor(won ? DISPLAY_GREEN : DISPLAY_RED);
  display_setTextSize(TITLE_TEXT_SIZE);
  display_setCursor(
      HALF_WIDTH -
          (won ? GAME_OVER_TITLE_WIN_LENGTH : GAME_OVER_TITLE_LOSE_LENGTH) *
              TITLE_CHAR_SIZE_WIDTH,
      HALF_HEIGHT - TITLE_CHAR_SIZE_HEIGHT - TITLE_CHAR_SIZE_HEIGHT);
  display_print(won ? GAME_OVER_TITLE_WIN : GAME_OVER_TITLE_LOSE);

  // show score
  display_setTextColor(DISPLAY_WHITE);
  display_setTextSize(TEXT_SIZE);
  display_setCursor(HALF_WIDTH -
                        (GAME_OVER_TEXT_1a_LENGTH + GAME_OVER_TEXT_1b_LENGTH) *
                            CHAR_SIZE_WIDTH,
                    HALF_HEIGHT);
  display_print(GAME_OVER_TEXT_1a);
  display_printDecimalInt(iterationLength);
  display_print(GAME_OVER_TEXT_1b);
  display_printDecimalInt(sequenceLength);

  // "Tap to play again :)"
  display_setTextColor(DISPLAY_YELLOW);
  display_setCursor(HALF_WIDTH - GAME_OVER_TEXT_2_LENGTH * CHAR_SIZE_WIDTH,
                    HALF_HEIGHT + CHAR_SIZE_HEIGHT + CHAR_SIZE_HEIGHT);
  display_print(GAME_OVER_TEXT_2);
}

// output the currentState
void simonControl_printStateString() {
  static enum sc_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {

    // output thingy
    switch (curState) {
    case init_st:
      printf("init_st\n");
      break;
    case main_st:
      printf("main_st\n");
      break;
    case flash_st:
      printf("flash_st\n");
      break;
    case verify_st:
      printf("verify_st\n");
      break;
    case game_over_st:
      printf("game_over_st\n");
      break;
    default:
      printf("ERROR!!! UNACCEPTABLE STATE IN simonControl debug state!\n");
      break;
    }
  }
  prevState = curState;
}

// Used to init the state machine. Always provided though it may not be
// necessary.
void simonControl_init() {
  curState = init_st;
  sequenceLength = DEFAULT_SEQUENCE_LENGTH;
  iterationLength = DEFAULT_ITERATION_LENGTH;
  sc_counter = 0;
  sc_enabled = false;
}

// Standard tick function.
void simonControl_tick() {
  // show state changes if we want to
  if (SHOW_DEBUG) {
    simonControl_printStateString();
  }

  // transition switch
  switch (curState) {
  case init_st:
    // if enabled, go to main screen
    if (sc_enabled) {
      display_fillScreen(DISPLAY_BLACK);
      simonControl_drawMainScreen();
      display_clearOldTouchData();
      curState = main_st;
    }
    break;
  case main_st:
    // if disabled, init
    if (!sc_enabled) {
      simonControl_init();
    }

    // if touch, init sequence, clear screen, enable flash, transition to flash
    if (display_isTouched()) {
      display_fillScreen(DISPLAY_BLACK);
      simonControl_setSequenceLength(DEFAULT_SEQUENCE_LENGTH);
      flashSequence_enable();
      sc_counter = 0;
      curState = flash_st;
    }
    break;
  case flash_st:
    // if disabled, init
    if (!sc_enabled) {
      simonControl_init();
    }

    // if flash completed, disable flash, enable verify, transition to verify
    if (flashSequence_isComplete()) {
      flashSequence_disable();
      verifySequence_enable();
      curState = verify_st;
    }
    break;
  case verify_st:
    // if disabled, init
    if (!sc_enabled) {
      simonControl_init();
    }

    // if verify complete and correct and index < length - 1, increment
    // iteration index, disable verify, enable flash, transition to flash
    if (verifySequence_isComplete() && !verifySequence_isTimeOutError() &&
        !verifySequence_isUserInputError() &&
        iterationLength < sequenceLength) {
      iterationLength++;
      simonDisplay_eraseAllButtons();
      globals_setSequenceIterationLength(iterationLength);
      verifySequence_disable();
      flashSequence_enable();
      curState = flash_st;
      // else if verify complete and (incorrect or iteration index >= length -
      // 1), clear screen, draw gameover text, disable verify, transition to
      // game over
    } else if (verifySequence_isComplete()) {
      simonDisplay_eraseAllButtons();
      simonControl_drawGameOverScreen();
      display_clearOldTouchData();
      curState = game_over_st;
    }
    break;
  case game_over_st:
    // if disabled, init
    if (!sc_enabled) {
      simonControl_init();
    }

    // if touch && correct, disable verify, increment length, clear screen,
    // transition to flash
    if (display_isTouched() && !verifySequence_isTimeOutError() &&
        !verifySequence_isUserInputError()) {
      verifySequence_disable();
      simonControl_setSequenceLength(++sequenceLength);
      display_fillScreen(DISPLAY_BLACK);
      flashSequence_enable();
      sc_counter = 0;
      curState = flash_st;
      // if touch && incorrect, disable verify set length to four, clear screen,
      // enable flash && transition to flash
    } else if (display_isTouched() && (verifySequence_isTimeOutError() ||
                                       verifySequence_isUserInputError())) {
      verifySequence_disable();
      sequenceLength = DEFAULT_SEQUENCE_LENGTH;
      simonControl_setSequenceLength(sequenceLength);
      display_fillScreen(DISPLAY_BLACK);
      flashSequence_enable();
      sc_counter = 0;
      curState = flash_st;
      // else if sc_counter limit reached, init
    } else if (sc_counter > GAME_OVER_COUNTER_LIMIT) {
      curState = init_st;
      sequenceLength = DEFAULT_SEQUENCE_LENGTH;
      iterationLength = DEFAULT_ITERATION_LENGTH;
      sc_counter = 0;
    }
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN simonControl transition state!\n");
    break;
  }

  // action switch
  switch (curState) {
  case init_st:
    // nothing
    break;
  case main_st:
    // sc_counter for rand seed
    sc_counter++;
    break;
  case flash_st:
    // nothing
    break;
  case verify_st:
    // nothing
    break;
  case game_over_st:
    // sc_counter
    sc_counter++;
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN simonControl action state!\n");
    break;
  }
}
