#include "lockoutTimer.h"

#define RESET_VALUE 0

typedef enum {
  init_st,
  timer_st,
  finished_st,
} lock_st_t;

static lock_st_t currentState;
static bool start;
static uint32_t timer;

// Calling this starts the timer.
void lockoutTimer_start() { start = true; }

// Perform any necessary inits for the lockout timer.
void lockoutTimer_init() { currentState = init_st; }

// Returns true if the timer is running.
bool lockoutTimer_running() { return (timer != RESET_VALUE); }

// Standard tick function.
void lockoutTimer_tick() {
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
    if (timer == LOCKOUT_TIMER_EXPIRE_VALUE) {
      currentState = finished_st;
    }
    break;

  case finished_st:
    start = false;
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
bool lockoutTimer_runTest() {}
