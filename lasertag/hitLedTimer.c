#include "hitLedTimer.h"
#include "leds.h"
#include "mio.h"

// The lockoutTimer is active for 1/2 second once it is started.
// It is used to lock-out the detector once a hit has been detected.
// This ensure that only one hit is detected per 1/2-second interval.

#define HIT_LED_TIMER_EXPIRE_VALUE 50000 // Defined in terms of 100 kHz ticks.
#define HIT_LED_TIMER_OUTPUT_PIN 11      // JF-3
#define RESET_VALUE 0
#define MAX_TIMER_COUNT 50000
#define TURN_ON_LED0 0x01
#define TURN_OFF_LED0 0x00
#define HIGH 1
#define LOW 0

typedef enum {
  init_st,
  illuminate_st,
  finished_st,
} led_st_t;

static led_st_t currentState;
static bool enable;
volatile static bool timerStartFlag;
static uint32_t timer;

// Need to init things.
void hitLedTimer_init() {
  // Initialize the leds.
  leds_init(false);
  currentState = init_st;
  timer = RESET_VALUE;
}

// Calling this starts the timer.
void hitLedTimer_start() { timerStartFlag = true; }

// Returns true if the timer is currently running.
bool hitLedTimer_running() { return (timer != RESET_VALUE); }

// Turns the gun's hit-LED on.
void hitLedTimer_turnLedOn() {
  // Illuminate LED LD0.
  leds_write(TURN_ON_LED0);
  // turn on other LEDs.
  mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, HIGH);
}

// Turns the gun's hit-LED off.
void hitLedTimer_turnLedOff() {
  // Turn off LED LD00.
  leds_write(TURN_OFF_LED0);
  // turn off other LEDs.
  mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, LOW);
}

// Standard tick function.
void hitLedTimer_tick() {
  // Transition actions.
  switch (currentState) {
  case init_st:
    // Check to see if the enable flag is on.
    if (enable && timerStartFlag) {
      // Illuminate LED LD0.
      leds_write(TURN_ON_LED0);
      currentState = illuminate_st;
      timer = RESET_VALUE;
    }
    break;

  case illuminate_st:
    // Check to see if the enable flag is on.
    if (!enable) {
      currentState = finished_st;
    }
    // Check to see if the timer is up.
    else if (timer == MAX_TIMER_COUNT) {
      // Turn off LED LD00.
      leds_write(TURN_OFF_LED0);
      currentState = finished_st;
    }

  case finished_st:
    // Check to see if the enable flag is on.
    if (enable) {
      currentState = init_st;
    }
  }

  // State actions.
  switch (currentState) {
  case init_st:
    // No actions.
    break;
  case illuminate_st:
    // Increment timer.
    timer++;
  case finished_st:
    timerStartFlag = false;
    break;
  }
}

// Disables the hitLedTimer.
void hitLedTimer_disable() { enable = false; }

// Enables the hitLedTimer.
void hitLedTimer_enable() { enable = true; }

// Runs a visual test of the hit LED.
// The test continuously blinks the hit-led on and off.
void hitLedTimer_runTest() {}
