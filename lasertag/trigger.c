#include "trigger.h"
#include "buttons.h"
#include "mio.h"
#include "transmitter.h"
#include <stdio.h>

#define MAX_TIMER_COUNT 5000
#define TRIGGER_GUN_TRIGGER_MIO_PIN 10
#define GUN_TRIGGER_PRESSED 1
#define RESET_TIMER 0
#define DEBUG true

// state machine
typedef enum {
  init_st,
  wait_st,
  press_debounce_st,
  transmit_st,
  release_debounce_st,
  finished_st,
} trigger_st_t;
static trigger_st_t currentState;

static bool enable;
static bool ignoreGunInput;
static uint32_t timer;
static uint16_t remainingShotCount;
volatile static bool finishedDebounce = false;

// Trigger can be activated by either btn0 or the external gun that is attached
// to TRIGGER_GUN_TRIGGER_MIO_PIN Gun input is ignored if the gun-input is high
// when the init() function is invoked.
bool triggerPressed() {
  return ((!ignoreGunInput &
           (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) ||
          (buttons_read() & BUTTONS_BTN0_MASK));
}

// Init trigger data-structures.
// Determines whether the trigger switch of the gun is connected (see discussion
// in lab web pages). Initializes the mio subsystem.
void trigger_init() {
  // false disables any debug printing if there is a system failure during init.
  mio_init(false);
  buttons_init(); // Using buttons
  // Set the current state to initial state.
  mio_setPinAsInput(TRIGGER_GUN_TRIGGER_MIO_PIN);
  // If the trigger is pressed when trigger_init() is called, assume that the
  // gun is not connected and ignore it.
  if (triggerPressed()) {
    ignoreGunInput = true;
  }
  currentState = init_st;
  timer = RESET_TIMER;
}

// Enable the trigger state machine. The trigger state-machine is inactive until
// this function is called. This allows you to ignore the trigger when helpful
// (mostly useful for testing).
void trigger_enable() {
  // Enable the state machine.
  enable = true;
}

// Disable the trigger state machine so that trigger presses are ignored.
void trigger_disable() {
  // Disable the state mac
  enable = false;
}

// Returns the number of remaining shots.
trigger_shotsRemaining_t trigger_getRemainingShotCount() {
  return remainingShotCount;
}

// Sets the number of remaining shots.
void trigger_setRemainingShotCount(trigger_shotsRemaining_t count) {
  remainingShotCount = count;
}

// debug state machine
void trigger_debug() {
  static trigger_st_t prevState = init_st;

  // check if they're different
  if (prevState != currentState && enable) {
    // output them if they are
    switch (currentState) {
    case transmit_st:
      printf("D\n");
      break;
    case finished_st:
      printf("U\n");
      break;
    }

    prevState = currentState;
  }
}

// Standard tick function.
void trigger_tick() {
  // output if debug
  if (DEBUG) {
    trigger_debug();
  }

  // Transition switches.
  switch (currentState) {
  case init_st:
    // Check to see if the enable flag is on.
    if (enable) {
      finishedDebounce = false;
      currentState = wait_st;
    }
    break;

  case wait_st:
    // Check to see if the enable flag is on.
    if (!enable) {
      currentState = finished_st;
    } else {
      // Check to see if the trigger is pressed.
      if (triggerPressed()) {
        currentState = press_debounce_st;
      }
    }
    break;

  case press_debounce_st:
    // Check to see if the enable flag is on.
    if (!enable) {
      currentState = finished_st;
    } else {
      // Check to see if the trigger is pressed.
      if (!triggerPressed()) {
        // currentState = wait_st;
        timer = RESET_TIMER;
      }
      // If the debounce timer is up, start transmitting.
      else if (timer >= MAX_TIMER_COUNT) {
        currentState = transmit_st;
        // Start the transmitter.
        transmitter_run();
      }
    }
    break;

  case transmit_st:
    // Check to see if the enable flag is on.
    if (!enable) {
      currentState = finished_st;
    } else {
      // Check to see if the trigger is still pressed.
      if (!triggerPressed()) {
        currentState = release_debounce_st;
        remainingShotCount--;
        timer = RESET_TIMER;
      }
    }
    break;

  case release_debounce_st:
    // Check to see if the enable flag is on.
    if (!enable) {
      currentState = finished_st;
    } else {
      // Check to see if the trigger is pressed.
      if (triggerPressed()) {
        // currentState = transmit_st;
        timer = RESET_TIMER;
      }
      // If the debounce timer is up, start transmitting.
      else if (timer == MAX_TIMER_COUNT) {
        currentState = finished_st;
      }
    }
    break;

  case finished_st:
    // If the trigger is enabled, move to the initial state.
    if (enable) {
      currentState = init_st;
    }
    break;
  }

  // State actions.
  switch (currentState) {
  case init_st:
    // No actions.
    break;
  case wait_st:
    // No actions.
    break;
  case press_debounce_st:
    // Increment timer.
    timer++;
    break;
  case transmit_st:
    // No actions.
    break;
  case release_debounce_st:
    // Increment timer.
    timer++;
    break;
  finished_st:
    finishedDebounce = true;
    timer = RESET_TIMER;
    break;
  }
}

// Runs the test continuously until BTN1 is pressed.
// The test just prints out a 'D' when the trigger or BTN0
// is pressed, and a 'U' when the trigger or BTN0 is released.
void trigger_runTest() {
  // Enable the trigger.
  printf("\n\n==========================================\nStarted trigger "
         "test. \nPress button 1 to finish the test.\nPress button 0 to test "
         "debounce.\n");

  // perform test until button 1 is pressed
  while (!(buttons_read() & BUTTONS_BTN1_MASK)) {
    trigger_enable();
  }
  trigger_disable();
  printf("Ended trigger test.\n==========================================\n\n");
}
