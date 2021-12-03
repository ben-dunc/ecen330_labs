// Created by Benjamin Duncan - Dec 2, 2021

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pongControl.h"
#include "gameControl.h"
#include "display.h"

#define SHOW_DEBUG true

static bool isGameOver = false;
static bool isEnabled = false;

// enuming!
enum pc_st_t {
    init_st,
    start_st,
    get_diff_st,
    game_st,
    gameOver_st
};
static enum pc_st_t curState;

// renders start screen
void pongControl_renderStartScreen() {
    display_fillScreen(DISPLAY_BLACK);
    display_setCursor(0, 0);
    display_setTextSize(3);
    display_setTextColor(DISPLAY_WHITE);
    display_println("hello! This is the start screen.");
}

// renders gameover screen
void pongControl_renderGameOverScreen() {
    display_fillScreen(DISPLAY_BLACK);
    display_setCursor(0, 0);
    display_setTextSize(3);
    display_setTextColor(DISPLAY_GREEN);
    display_println("good bye! This is the gameover screen.");
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
        case get_diff_st:
            printf("get_diff_st\n");
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
                curState = get_diff_st;
            }
            break;
        case get_diff_st:
            // go to init if disabled
            if (!isEnabled) {
                pongControl_init();
            } else if (display_isTouched()) {
                // get diff values
                curState = game_st;
            } else {
                curState = start_st;
            }
            break;
        case game_st:
            // go to init if disabled
            if (!isEnabled) {
                pongControl_init();
            } else if (gameControl_isComplete()) {
                curState = gameOver_st;
                pongControl_renderGameOverScreen();
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
        case get_diff_st:
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
void pongControl_enable() {
    isEnabled = true;
}

// sets the enabled flag low
void pongControl_disable() {
    isEnabled = false;
}

// returns if the state machine is complete
bool pongControl_isGameOver() {
    return isGameOver;
}
