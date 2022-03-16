#include "hitLedTimer.h"
#include "buttons.h"
#include "leds.h"
#include "mio.h"
#include "utils.h"
#include <stdio.h>

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
#define TEST_DELAY_MS 300

// state machine
typedef enum {
  init_st,
  illuminate_st,
  wait_st,
} led_st_t;

static led_st_t currentState;
//static bool enable;
volatile static bool timerStartFlag;
static uint32_t timer;

// Need to init things.
void hitLedTimer_init() {
  // Initialize the leds.
  //leds_init(false);
  // false disables any debug printing if there is a system failure during init.
  //mio_init(false);
  // Configure the signal direction of the pin to be an output.
  mio_setPinAsOutput(HIT_LED_TIMER_OUTPUT_PIN);
  currentState = init_st;
  timer = RESET_VALUE;
}

// Calling this starts the timer.
void hitLedTimer_start() { timerStartFlag = true; }

// Disables the hitLedTimer.
void hitLedTimer_disable() { timerStartFlag = false; }

// Enables the hitLedTimer.
void hitLedTimer_enable() { timerStartFlag = true; }

// Returns true if the timer is currently running.
bool hitLedTimer_running() { return timerStartFlag; }

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
    currentState = wait_st;
    break;

    case wait_st:
    // Check to see if the enable flag is on.
    if (timerStartFlag) {
      // Illuminate LED LD0.
      hitLedTimer_turnLedOn();
      currentState = illuminate_st;
      timer = RESET_VALUE;
    }
    break;

  case illuminate_st:
    // Check to see if the enable flag is on.
    if (!timerStartFlag) {
      currentState = wait_st;
      // Turn off LEDs.
      hitLedTimer_turnLedOff();
    }
    // Check to see if the timer is up.
    else if (timer >= MAX_TIMER_COUNT) {
      // Turn off LED LD00.
      hitLedTimer_turnLedOff();
      timerStartFlag = false;
      currentState = wait_st;
      timer = RESET_VALUE;
    }
    break;
  }

  // State actions.
  switch (currentState) {
  case init_st:
    // No actions.
    break;
  case illuminate_st:
    // Increment timer.
    timer++;
    break;
  case wait_st:
    break;
  }
}

// Runs a visual test of the hit LED.
// The test continuously blinks the hit-led on and off.
void hitLedTimer_runTest() {
  // Enable the hit led.
  printf(
      "\n\n==========================================\nHit LED test. \nPress "
      "button 0 to start the test. \nPress button 1 to finish the test.\n");
  hitLedTimer_enable();
  // Wait for button 0.
  while (!(buttons_read() & BUTTONS_BTN0_MASK))
    ;
  // wait for button 0 is depressed.
  while ((buttons_read() & BUTTONS_BTN0_MASK))
    ;

  // Run a test for flashing the led. wait for button 1 to finish test.
  while (!(buttons_read() & BUTTONS_BTN1_MASK)) {
    // Start the led flash.
    hitLedTimer_start();
    // wait for the led timer to finish.
    while (hitLedTimer_running())
      ;
    // wait 300 ms
    utils_msDelay(TEST_DELAY_MS);
  }

  printf("Ended hit LED test "
         "test.\n==========================================\n\n");
}
