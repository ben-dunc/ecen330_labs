// Created by Benjamin Duncan - Dec 2, 2021

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <display.h>
#include "gameControl.h"

#define DEBUG_SHOW true

#define SCORE_LIMIT             10
#define COUNTER_LIMIT           100

#define PIXEL_SIZE              5
#define HALF_PIXEL_SIZE         PIXEL_SIZE / 2
#define PADDLE_WIDTH            PIXEL_SIZE
#define PADDLE_LENGTH           PIXEL_SIZE * 4
#define PADDLE_HALF_LENGTH      PADDLE_LENGTH / 2
#define PADDLE_OFFSET           PIXEL_SIZE * 3
#define HALF_WIDTH              DISPLAY_WIDTH / 2
#define HALF_HEIGHT             DISPLAY_HEIGHT / 2

#define NUM_TABLE_SQUARES       DISPLAY_HEIGHT / PIXEL_SIZE
#define TABLE_SQUARES_OFFSET    (DISPLAY_HEIGHT - (NUM_TABLE_SQUARES * PIXEL_SIZE)) / 2

#define PLAYER_SPEED_LIMIT      100
#define COMP_EASY_SPEED_LIMIT   50
#define COMP_NORMAL_SPEED_LIMIT 100
#define COMP_HARD_SPEED_LIMIT   150


// global variables
static bool isComplete = false;
static bool isEnabled = false;
static bool playerServe = true;

static uint8_t counter = 0;
static uint8_t score1 = 0;
static uint8_t score2 = 0;

static int32_t ballVelX = 0;
static int32_t ballVelY = 0;
static int32_t ballPosX = 0;
static int32_t ballPosY = 0;

static int32_t paddle1PosX = PADDLE_OFFSET;
static int32_t paddle1PosY = HALF_HEIGHT - PADDLE_HALF_LENGTH;

static int32_t paddle2PosX = DISPLAY_WIDTH - PADDLE_OFFSET - PIXEL_SIZE;
static int32_t paddle2PosY = HALF_HEIGHT - PADDLE_HALF_LENGTH;

// enuming!
enum gc_st_t {
    init_st,
    serve_st,
    gameplay_st,
    complete_st,
};
static enum gc_st_t curState;


// render table
void gameControl_renderTable() {
    // draw new table (really, it's just the middle line)
    for (uint8_t i = 0; i < NUM_TABLE_SQUARES; i += 2) {
        display_fillRect(
            HALF_WIDTH - HALF_PIXEL_SIZE,
            TABLE_SQUARES_OFFSET + (PIXEL_SIZE * i),
            PIXEL_SIZE,
            PIXEL_SIZE,
            DISPLAY_WHITE
        );
    }
}

// render score
void gameControl_renderScore() {
    static uint8_t prevScore1 = 0; 
    static uint8_t prevScore2 = 0;

    // erase old score 

    // draw new score

    // assign new values
}

// render ball
void gameControl_renderBall() {
    static int32_t prevX = 0;
    static int32_t prevY = 0;

    // erase prev x and y

    // draw new ball

    // assign new values
}

// render paddles
void gameControl_renderPaddles() {

}

// output the currentState
void gameControl_printStateString() {
  static enum gc_st_t prevState = init_st;

  // check if there is a new state
  if (prevState != curState) {

    // output thingy
    switch (curState) {
        case init_st:
            printf("\tinit_st\n");
            break;
        case serve_st:
            printf("\tserve_st\n");
            break;
        case gameplay_st:
            printf("\tgameplay_st\n");
            break;
        case complete_st:
            printf("\tcomplete_st\n");
            break;
        default:
            printf("ERROR!!! UNACCEPTABLE STATE IN gameController debug state!\n");
            break;
    }
  }

  prevState = curState;
}

// inits the game state function
void gameControl_init() {
    curState = init_st;
    counter = 0;
    isComplete = false;
}

// ticks the game state function
void gameControl_tick() {
    if (DEBUG_SHOW) {
        gameControl_printStateString();
    }

    // transition state
    switch (curState) {
        case init_st:
            // start if enabled
            if (isEnabled) {
                // TEST
                gameControl_renderTable();
                gameControl_renderScore();
                gameControl_renderBall();
                gameControl_renderPaddles();

                // END TEST
                curState = serve_st;
            }
            break;
        case serve_st:
            // init if disabled
            if (!isEnabled) {
                gameControl_init();
            } else if (counter > COUNTER_LIMIT) {
                // serve
                counter = 0;
                curState = gameplay_st;
            }


            break;
        case gameplay_st:
            // init if disabled
            if (!isEnabled) {
                gameControl_init();
            }  else if (score1 >= SCORE_LIMIT || score2 >= SCORE_LIMIT) {
                isComplete = true;
                curState = complete_st;
            }
            isComplete = true;
            curState = complete_st;


            break;
        case complete_st:
            // init if disabled
            if (!isEnabled) {
                gameControl_init();
            }
            // wait for disable 
            break;
        default:
            printf("ERROR!!! UNACCEPTABLE STATE IN gameController debug state!\n");
            break;
    }

    // transition state
    switch (curState) {
        case init_st:
            break;
        case serve_st:
            counter++;
            break;
        case gameplay_st:
            break;
        case complete_st:
            break;
        default:
            printf("ERROR!!! UNACCEPTABLE STATE IN gameController debug state!\n");
            break;
    }
}

// sets the enabled flag high
void gameControl_enable() {
    isEnabled = true;
}

// sets the enabled flag low
void gameControl_disable() {
    isEnabled = false;
}

// returns if the state machine is complete
bool gameControl_isComplete() {
    return isComplete;
}