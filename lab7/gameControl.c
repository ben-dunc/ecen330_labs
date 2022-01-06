// Created by Benjamin Duncan - Dec 2, 2021

#include "gameControl.h"
#include "buttons.h"
#include "globals.h"
#include <display.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG_SHOW true

#define SCORE_LIMIT 10
#define COUNTER_LIMIT 20

#define PIXEL_SIZE 5
#define HALF_PIXEL_SIZE PIXEL_SIZE / 2
#define PADDLE_WIDTH PIXEL_SIZE
#define PADDLE_LENGTH PIXEL_SIZE * 10
#define PADDLE_HALF_LENGTH PADDLE_LENGTH / 2
#define PADDLE_OFFSET PIXEL_SIZE
#define HALF_WIDTH DISPLAY_WIDTH / 2
#define HALF_HEIGHT DISPLAY_HEIGHT / 2

#define PADDLE1_X PADDLE_OFFSET
#define PADDLE2_X DISPLAY_WIDTH - PADDLE_OFFSET - PIXEL_SIZE

#define SCORE_CHAR_SIZE 4
#define SCORE_CHAR_WIDTH SCORE_CHAR_SIZE *DISPLAY_CHAR_WIDTH
#define SCORE_CHAR_HEIGHT SCORE_CHAR_SIZE *DISPLAY_CHAR_HEIGHT
#define SCORE1_OFFSET HALF_WIDTH - SCORE_CHAR_WIDTH - (PIXEL_SIZE * 2)
#define SCORE2_OFFSET HALF_WIDTH + (PIXEL_SIZE * 4)

#define NUM_TABLE_SQUARES DISPLAY_HEIGHT / PIXEL_SIZE
#define TABLE_SQUARES_OFFSET                                                   \
  (DISPLAY_HEIGHT - (NUM_TABLE_SQUARES * PIXEL_SIZE)) / 2

#define PLAYER_SPEED_LIMIT 5
#define COMP_SPEED 2
#define COMP_SPEED_FACTOR 2
#define COLLISION_FACTOR 0.5
#define BALL_SPEED_FACTOR 1.5

#define BALL_SERVE_SPEED 5

// global variables
static bool isComplete = false;
static bool isEnabled = false;
static bool playerServe = true;

static uint8_t counter = 0;
static uint8_t score1 = 0;
static uint8_t score2 = 0;

static int32_t ballVelX = 0;
static int32_t ballVelY = 0;
static int32_t ballPosX = 100;
static int32_t ballPosY = 100;

static int32_t paddle1PosY = HALF_HEIGHT - PADDLE_HALF_LENGTH;
static int32_t paddle2PosY = HALF_HEIGHT - PADDLE_HALF_LENGTH;

// enuming!
enum gc_st_t {
  init_st,
  serve_st,
  gameplay_st,
  complete_st,
};
static enum gc_st_t curState;

// render table - called after rendering the ball
void gameControl_renderTable(bool force) {
  // draw new table (really, it's just the middle line)
  if (HALF_WIDTH - PIXEL_SIZE - PIXEL_SIZE < ballPosX &&
          ballPosX < HALF_WIDTH + PIXEL_SIZE + PIXEL_SIZE ||
      force) {
    // draw thingies
    for (uint8_t i = 0; i < NUM_TABLE_SQUARES; i += 2) {
      display_fillRect(HALF_WIDTH - HALF_PIXEL_SIZE,
                       TABLE_SQUARES_OFFSET + (PIXEL_SIZE * i), PIXEL_SIZE,
                       PIXEL_SIZE, DISPLAY_WHITE);
    }
  }
}

// render score - called at the same time as render ball
void gameControl_renderScore(bool force) {
  static uint8_t prevScore1 = 0;
  static uint8_t prevScore2 = 0;
  char output[10];

  display_setTextSize(SCORE_CHAR_SIZE);

  // score 1 - do stuff if change
  if (prevScore1 != score1 ||
      (ballPosX > SCORE1_OFFSET && ballPosX < HALF_WIDTH &&
       ballPosY < PADDLE_OFFSET + SCORE_CHAR_HEIGHT) ||
      force) {
    // erase old score
    display_setCursor(SCORE1_OFFSET, PADDLE_OFFSET);
    display_setTextColor(DISPLAY_BLACK);
    sprintf(output, "%d", prevScore1);
    display_print(output);

    // draw new score
    display_setCursor(SCORE1_OFFSET, PADDLE_OFFSET);
    display_setTextColor(DISPLAY_WHITE);
    sprintf(output, "%d", score1);
    display_print(output);

    // assign new values
    prevScore1 = score1;
  }

  // score 2 - do stuff if change
  if (prevScore2 != score2 ||
      (ballPosX > SCORE2_OFFSET &&
       ballPosX < HALF_WIDTH + SCORE2_OFFSET + SCORE_CHAR_WIDTH &&
       ballPosY < PADDLE_OFFSET + SCORE_CHAR_HEIGHT) ||
      force) {
    // erase old score
    display_setCursor(SCORE2_OFFSET, PADDLE_OFFSET);
    display_setTextColor(DISPLAY_BLACK);
    sprintf(output, "%d", prevScore2);
    display_print(output);

    // draw new score
    display_setCursor(SCORE2_OFFSET, PADDLE_OFFSET);
    display_setTextColor(DISPLAY_WHITE);
    sprintf(output, "%d", score2);
    display_print(output);

    // assign new values
    prevScore2 = score2;
  }
}

// render ball - called at the same time as rendering paddles
void gameControl_renderBall(bool force) {
  static int32_t prevX = 0;
  static int32_t prevY = 0;

  // if there is a change
  if (prevX != ballPosX || prevY != prevY || force) {
    // erase prev x and y
    display_fillRect(prevX, prevY, PIXEL_SIZE, PIXEL_SIZE, DISPLAY_BLACK);

    // draw new ball
    display_fillRect(ballPosX, ballPosY, PIXEL_SIZE, PIXEL_SIZE, DISPLAY_WHITE);

    // assign new values
    prevX = ballPosX;
    prevY = ballPosY;
  }
}

// render paddles
void gameControl_renderPaddles(bool force) {
  static int32_t prevY1 = 0;
  static int32_t prevY2 = 0;

  // paddle 1 - do stuff if change
  if (prevY1 != paddle1PosY || force) {
    // erase paddle
    display_fillRect(PADDLE1_X, prevY1, PADDLE_WIDTH, PADDLE_LENGTH,
                     DISPLAY_BLACK);

    // draw paddle
    display_fillRect(PADDLE1_X, paddle1PosY, PADDLE_WIDTH, PADDLE_LENGTH,
                     DISPLAY_WHITE);

    // update prev value
    prevY1 = paddle1PosY;
  }

  // paddle 2 - do stuff if change
  if (prevY2 != paddle2PosY || force) {
    // erase paddle
    display_fillRect(PADDLE2_X, prevY2, PADDLE_WIDTH, PADDLE_LENGTH,
                     DISPLAY_BLACK);

    // draw paddle
    display_fillRect(PADDLE2_X, paddle2PosY, PADDLE_WIDTH, PADDLE_LENGTH,
                     DISPLAY_WHITE);

    // update prev value
    prevY2 = paddle2PosY;
  }
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

// check ball collisions
void gameControl_checkCollisions() {
  // paddle 1 collision
  if (ballPosY > paddle1PosY && ballPosY < (paddle1PosY + PADDLE_LENGTH) &&
      ballPosX - PADDLE1_X < (PIXEL_SIZE + PADDLE_WIDTH) && ballVelX < 0) {
    ballVelX = -ballVelX;
    ballVelY =
        ((ballPosY + HALF_PIXEL_SIZE) - (paddle1PosY + PADDLE_HALF_LENGTH)) *
        (COLLISION_FACTOR + (globals_getDifficulty() / 10));
    // paddle 2 collision
  } else if (ballPosY > paddle2PosY &&
             ballPosY < (paddle2PosY + PADDLE_LENGTH) &&
             PADDLE2_X - ballPosX < PIXEL_SIZE + PIXEL_SIZE && ballVelX > 0) {
    ballVelX = -ballVelX;
    ballVelY =
        ((ballPosY + HALF_PIXEL_SIZE) - (paddle2PosY + PADDLE_HALF_LENGTH)) *
        (COLLISION_FACTOR + (globals_getDifficulty() / 10));
  }

  // top and bottom collision
  if (ballPosY < HALF_PIXEL_SIZE) {
    ballVelY = -ballVelY;
  } else if (DISPLAY_HEIGHT - ballPosY < HALF_PIXEL_SIZE + PIXEL_SIZE) {
    ballVelY = -ballVelY;
  }
}

// get player input
void gameControl_getPlayerInput() {
  // Go down
  if (buttons_read() & BUTTONS_BTN0_MASK && paddle2PosY > 0) {
    paddle2PosY -= PLAYER_SPEED_LIMIT;
  }
  // Go up
  if (buttons_read() & BUTTONS_BTN1_MASK &&
      paddle2PosY + PADDLE_LENGTH < DISPLAY_HEIGHT - PIXEL_SIZE) {
    paddle2PosY += PLAYER_SPEED_LIMIT;
  }

  // check if versus mode
  if (globals_getVerses()) {
    // Go down
    if (buttons_read() & BUTTONS_BTN2_MASK && paddle1PosY > 0) {
      paddle1PosY -= PLAYER_SPEED_LIMIT;
    }
    // Go down
    if (buttons_read() & BUTTONS_BTN3_MASK &&
        paddle1PosY + PADDLE_LENGTH < DISPLAY_HEIGHT - PIXEL_SIZE) {
      paddle1PosY += PLAYER_SPEED_LIMIT;
    }
  }
}

// CPU move
void gameControl_moveCPU() {
  // if difficulty is medium, play smart, not hard
  if (globals_getDifficulty() == DIFFICULTY_MEDIUM ||
      globals_getDifficulty() == DIFFICULTY_HARD) {
    // If we don't need to move, don't do anything
    if (ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH) > 0 && ballVelY < 0) {
      return;
    } else if (ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH) < 0 &&
               ballVelY > 0) {
      return;
    }
  }

  // check if difference is less than speed
  if (abs(ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH)) >
      COMP_SPEED + (COMP_SPEED_FACTOR * globals_getDifficulty())) {
    // move!
    if (ballPosY < (paddle1PosY + PADDLE_HALF_LENGTH) && paddle1PosY > 0) {
      paddle1PosY -=
          (COMP_SPEED + (COMP_SPEED_FACTOR * globals_getDifficulty()));
    } else if (ballPosY > (paddle1PosY + PADDLE_HALF_LENGTH) &&
               paddle1PosY + PADDLE_LENGTH < DISPLAY_HEIGHT) {
      paddle1PosY +=
          (COMP_SPEED + (COMP_SPEED_FACTOR * globals_getDifficulty()));
    }
  } else {
    // move!
    if (ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH) > 0 &&
        paddle1PosY + PADDLE_LENGTH < DISPLAY_HEIGHT - PIXEL_SIZE) {
      paddle1PosY += ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH);
    } else if (ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH) < 0 &&
               paddle1PosY > 0) {
      paddle1PosY += ballPosY - (paddle1PosY + PADDLE_HALF_LENGTH);
    }
  }
}

// reset the ball
void gameControl_resetBall() {
  ballPosX = HALF_WIDTH + (playerServe ? SCORE_CHAR_WIDTH : -SCORE_CHAR_WIDTH);
  ballPosY = HALF_HEIGHT - HALF_PIXEL_SIZE;
  ballVelX = 0;
  ballVelY = 0;
}

// inits the game state function
void gameControl_init() {
  curState = init_st;
  counter = 0;
  isComplete = false;
  playerServe = true;
  score1 = 0;
  score2 = 0;
  gameControl_resetBall();
}

// ticks the game state function
void gameControl_tick() {
  // if we want to see the state transitions
  if (DEBUG_SHOW) {
    gameControl_printStateString();
  }

  // transition state
  switch (curState) {
  case init_st:
    // start if enabled
    if (isEnabled) {
      // force render
      gameControl_renderTable(true);
      gameControl_renderScore(true);
      gameControl_renderBall(true);
      gameControl_renderPaddles(true);

      curState = serve_st;
    }
    break;
  case serve_st:
    // init if disabled
    if (!isEnabled) {
      gameControl_init();
    } else if (counter > COUNTER_LIMIT) {
      // serve - alternate serve
      ballVelX = playerServe ? (BALL_SERVE_SPEED +
                                (globals_getDifficulty() * BALL_SPEED_FACTOR))
                             : -(BALL_SERVE_SPEED +
                                 (globals_getDifficulty() * BALL_SPEED_FACTOR));
      ballVelY = (score1 + score2) % 3 >= 1
                     ? (globals_getDifficulty() * BALL_SPEED_FACTOR)
                     : -(globals_getDifficulty() * BALL_SPEED_FACTOR);
      playerServe = !playerServe;

      // transition
      counter = 0;
      curState = gameplay_st;
    }
    break;
  case gameplay_st:
    // init if disabled
    if (!isEnabled) {
      gameControl_init();
      // check if point won
    } else if (ballPosX > DISPLAY_WIDTH || ballPosX < 0) {
      ballPosX < 0 ? score2++ : score1++;
      if (score1 == SCORE_LIMIT || score2 == SCORE_LIMIT) {
        globals_setPlayerWon(score2 == SCORE_LIMIT ? PLAYER_WON : COMPUTER_WON);
        isComplete = true;
        curState = complete_st;
      } else {
        gameControl_resetBall();
        gameControl_renderBall(true);
        gameControl_renderScore(true);
        curState = serve_st;
      }
    }
    break;
  case complete_st:
    // wait for disable
    if (!isEnabled) {
      gameControl_init();
    }
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
    gameControl_getPlayerInput();
    // get player input
    if (!globals_getVerses()) {
      gameControl_moveCPU();
    }

    // check collisions and update values
    gameControl_checkCollisions();

    ballPosX += ballVelX;
    ballPosY += ballVelY;

    // render
    gameControl_renderPaddles(false);
    gameControl_renderBall(false);
    gameControl_renderTable(false);
    gameControl_renderScore(false);
    break;
  case complete_st:
    break;
  default:
    printf("ERROR!!! UNACCEPTABLE STATE IN gameController debug state!\n");
    break;
  }
}

// sets the enabled flag high
void gameControl_enable() { isEnabled = true; }

// sets the enabled flag low
void gameControl_disable() { isEnabled = false; }

// returns if the state machine is complete
bool gameControl_isComplete() { return isComplete; }