#include "lockoutTimer.h"
#include "buttons.h"
#include "intervalTimer.h"
#include <stdint.h>
#include <stdio.h>

#define RESET_VALUE 0
#define DEBUG false

typedef enum {
  init_st,
  timer_st,
  finished_st,
} lock_st_t;

static lock_st_t currentState;
volatile static bool start;
static uint32_t timer;

// Calling this starts the timer.
void lockoutTimer_start() { start = true; }

// Perform any necessary inits for the lockout timer.
void lockoutTimer_init() { currentState = init_st; }

// Returns true if the timer is running.
bool lockoutTimer_running() { return start; }

void lockoutTimer_debug() {
  static lock_st_t prevState = finished_st;

  // if previous state and current state are different
  if (prevState != currentState) {
    // output thingy
    switch (currentState) {
      case init_st:
        printf("\tinit_st\n");
      break;
      case timer_st:
        printf("\ttimer_st\n");
      break;
      case finished_st:
        printf("\tfinished state\n");
      break;
    }

    prevState = currentState;
  }
}

// Standard tick function.
void lockoutTimer_tick() {
  //debug
  if (DEBUG) {
    lockoutTimer_debug();
  }

  // Transition actions.
  switch (currentState) {
  case init_st:
    // Check if the start flag is on.
    if (start) {
      currentState = timer_st;
      timer = RESET_VALUE;
    }
    break;

  case timer_st:
    // Check if the timer is up.
    if (timer >= LOCKOUT_TIMER_EXPIRE_VALUE) {
      start = false;
      currentState = finished_st;
    }
    break;

  case finished_st:
    currentState = init_st;
    break;
  }

  // State actions
  switch (currentState) {
  case init_st:
    // No actions.
    break;
  case timer_st:
    // Increment the timer.
    timer++;
    break;
  case finished_st:
    // No actions.
    break;
  }
}

// Test function assumes interrupts have been completely enabled and
// lockoutTimer_tick() function is invoked by isr_function().
// Prints out pass/fail status and other info to console.
// Returns true if passes, false otherwise.
// This test uses the interval timer to determine correct delay for
// the interval timer.
bool lockoutTimer_runTest() {
    // Enable the hit led.
  printf("\n\n==========================================\nStarted lockout test.\nPress button 1 to finish the test.\nPress button 0 to start test.\n");

  intervalTimer_init(INTERVAL_TIMER_TIMER_1);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
  
  
  // wait till button 0 is pressed
  while (!(buttons_read() & BUTTONS_BTN0_MASK));
  // wait till button 0 is depressed
  while ((buttons_read() & BUTTONS_BTN0_MASK));
  printf("Timer started\n");

  lockoutTimer_start();
  intervalTimer_start(INTERVAL_TIMER_TIMER_1);

  // what until the timer is done
  while (lockoutTimer_running());

  intervalTimer_stop(INTERVAL_TIMER_TIMER_1);

  double time = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1);
  printf("time spent: %f\n", time);


  printf("Ended trigger test.\n==========================================\n\n");
}
