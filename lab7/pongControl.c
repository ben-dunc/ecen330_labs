// Created by Benjamin Duncan - Dec 2, 2021

#include "pongControl.h"
#include "display.h"
#include "gameControl.h"
#include "globals.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define START_TITLE_X 100
#define START_TITLE_Y 20
#define PLAYER_WIN_X 200
#define PLAYER_LOSE_X 50
#define BTN_OFFSET 10
#define HALF_BTN_OFFSET (BTN_OFFSET / 2)
#define BTN_RADIUS 10
#define DIFF_BTN_WIDTH ((DISPLAY_WIDTH - (BTN_OFFSET * 4)) / 3)
#define HALF_HEIGHT DISPLAY_HEIGHT / 2

#define TEXT1_X 110
#define TEXT1_Y 97

#define TEXT2_X 23
#define TEXT2_Y 177

#define TEXT3_X 135
#define TEXT3_Y 177

#define TEXT4_X 228
#define TEXT4_Y 177

#define TITLE_TEXT_SIZE 5
#define TEXT_SIZE 3

#define FIRST_HEIGHT_THIRD ((DISPLAY_HEIGHT / 3))
#define SECOND_HEIGHT_THIRD ((DISPLAY_HEIGHT / 3) * 2)

#define FIRST_WIDTH_THIRD ((DISPLAY_WIDTH / 3))
#define SECOND_WIDTH_THIRD ((DISPLAY_WIDTH / 3) * 2)

#define SHOW_DEBUG true

static bool isGameOver = false;
static bool isEnabled = false;

// enuming!
enum pc_st_t { init_st, start_st, get_mode_st, game_st, gameOver_st };
static enum pc_st_t curState;

// renders start screen
void pongControl_renderStartScreen() {
  // Title
  display_fillScreen(DISPLAY_BLACK);
  display_setCursor(START_TITLE_X, START_TITLE_Y);
  display_setTextSize(TITLE_TEXT_SIZE);
  display_setTextColor(DISPLAY_WHITE);
  display_println("PONG");

  // VS button
  display_drawRoundRect(
      BTN_OFFSET,
      FIRST_HEIGHT_THIRD + HALF_BTN_OFFSET - BTN_OFFSET - HALF_BTN_OFFSET,
      DISPLAY_WIDTH - BTN_OFFSET - BTN_OFFSET,
      FIRST_HEIGHT_THIRD - HALF_BTN_OFFSET, BTN_RADIUS, DISPLAY_WHITE);
  display_setCursor(TEXT1_X, TEXT1_Y);
  display_setTextSize(TEXT_SIZE);
  display_print("VERSES");

  // Diff button 1
  display_drawRoundRect(BTN_OFFSET,
                        SECOND_HEIGHT_THIRD + HALF_BTN_OFFSET - BTN_OFFSET -
                            HALF_BTN_OFFSET,
                        DIFF_BTN_WIDTH, FIRST_HEIGHT_THIRD - HALF_BTN_OFFSET,
                        BTN_RADIUS, DISPLAY_GREEN);
  display_setCursor(TEXT2_X, TEXT2_Y);
  display_print("EASY");

  // Diff button 2
  display_drawRoundRect(BTN_OFFSET + DIFF_BTN_WIDTH + BTN_OFFSET,
                        SECOND_HEIGHT_THIRD + HALF_BTN_OFFSET - BTN_OFFSET -
                            HALF_BTN_OFFSET,
                        DIFF_BTN_WIDTH, FIRST_HEIGHT_THIRD - HALF_BTN_OFFSET,
                        BTN_RADIUS, DISPLAY_YELLOW);
  display_setCursor(TEXT3_X, TEXT3_Y);
  display_print("MED");

  // Diff button 3
  display_drawRoundRect(
      BTN_OFFSET + DIFF_BTN_WIDTH + BTN_OFFSET + DIFF_BTN_WIDTH + BTN_OFFSET,
      SECOND_HEIGHT_THIRD + HALF_BTN_OFFSET - BTN_OFFSET - HALF_BTN_OFFSET,
      DIFF_BTN_WIDTH, FIRST_HEIGHT_THIRD - HALF_BTN_OFFSET, BTN_RADIUS,
      DISPLAY_RED);
  display_setCursor(TEXT4_X, TEXT4_Y);
  display_print("HARD");
}

// renders gameover screen
void pongControl_renderGameOverScreen() {
  // Title
  display_setCursor(globals_getPlayerWon() ? PLAYER_WIN_X : PLAYER_LOSE_X,
                    HALF_HEIGHT - BTN_OFFSET - BTN_OFFSET);
  display_setTextSize(TITLE_TEXT_SIZE);
  display_setTextColor(DISPLAY_WHITE);
  display_println("WIN");
}

// output the currentState
void pongControl_printStateString() {
  static enum pc_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {

    // output thingy
    switch (curState) {
    case init_st:
      printf("init_st\n");
      break;
    case start_st:
      printf("start_st\n");
      break;
    case get_mode_st:
      printf("get_mode_st\n");
      break;
    case game_st:
      printf("game_st\n");
      break;
    case gameOver_st:
      printf("gameOver_st\n");
      break;
    default:
      printf("ERROR!!! UNACCEPTABLE STATE IN pongController debug state!\n");
      break;
    }
  }

  prevState = curState;
}

// inits the pong control state machine
void pongControl_init() {
  isGameOver = false;
  curState = init_st;
}

// ticks the pong control state machine
void pongControl_tick() {
  // show debug stuffs
  if (SHOW_DEBUG) {
    pongControl_printStateString();
  }

  // transfer switch
  switch (curState) {
  case init_st:
    // go to init if disabled
    if (isEnabled) {
      curState = start_st;
      pongControl_renderStartScreen();
    }
    break;
  case start_st:
    // go to init if disabled
    if (!isEnabled) {
      pongControl_init();
    } else if (display_isTouched()) {
      display_clearOldTouchData();
      curState = get_mode_st;
    }
    break;
  case get_mode_st:
    // go to init if disabled
    if (!isEnabled) {
      pongControl_init();
    } else {
      // get mode
      int16_t x, y;
      uint8_t z;
      display_getTouchedPoint(&x, &y, &z);

      // verses
      if (FIRST_HEIGHT_THIRD < y && y < SECOND_HEIGHT_THIRD) {
        globals_setDifficulty(DIFFICULTY_HARD);
        globals_setVerses(true);
        display_fillScreen(DISPLAY_BLACK);
        gameControl_enable();
        curState = game_st;
        // diff
      } else if (SECOND_HEIGHT_THIRD < y && y < DISPLAY_HEIGHT) {
        // get diff
        if (0 < x && x < FIRST_WIDTH_THIRD) {
          globals_setDifficulty(DIFFICULTY_EASY);
        } else if (FIRST_WIDTH_THIRD < x && x < SECOND_WIDTH_THIRD) {
          globals_setDifficulty(DIFFICULTY_MEDIUM);
        } else if (SECOND_WIDTH_THIRD < x && x < DISPLAY_WIDTH) {
          globals_setDifficulty(DIFFICULTY_HARD);
        }

        globals_setVerses(false);
        display_fillScreen(DISPLAY_BLACK);
        gameControl_enable();
        curState = game_st;
      } else {
        curState = start_st;
      }
    }
    break;
  case game_st:
    // go to init if disabled
    if (!isEnabled) {
      pongControl_init();
    } else if (gameControl_isComplete()) {
      gameControl_disable();
      pongControl_renderGameOverScreen();
      curState = gameOver_st;
    }
    break;
  case gameOver_st:
    // go to init if disabled
    if (!isEnabled || display_isTouched()) {
      pongControl_init();
    }
    break;
  default:
    printf("ERROR!!! Unacceptable state in pongController transfer state!\n");
    break;
  }

  // action switch
  switch (curState) {
  case init_st:
    // do nothing
    break;
  case start_st:
    // do nothing
    break;
  case get_mode_st:
    // do nothing
    break;
  case game_st:
    // do nothing
    break;
  case gameOver_st:
    // do nothing
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN pongController action state!\n");
    break;
  }
}

// sets the enabled flag high
void pongControl_enable() { isEnabled = true; }

// sets the enabled flag low
void pongControl_disable() { isEnabled = false; }

// returns if the state machine is complete
bool pongControl_isGameOver() { return isGameOver; }
