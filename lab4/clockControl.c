/*
    Created by Benjamin Duncan - Oct 11, 2021
*/

#include "clockControl.h"
#include "clockDisplay.h"
#include <display.h>
#include <stdio.h>

#define TICKS_PER_SECOND 20
#define ADC_COUNTER_MAX_VALUE 1
#define AUTO_COUNTER_MAX_VALUE 10
#define RATE_COUNTER_MAX_VALUE 1

// States for the controller state machine.
enum clockControl_st_t {
  init_st, // Start here, transition out of this state on the first tick.
  never_touched_st, // Wait here until the first touch - clock is disabled until
                    // set.
  waiting_for_touch_st,  // waiting for touch, clock is enabled and running.
  adc_timer_running_st,  // waiting for the touch-controller ADC to settle.
  auto_timer_running_st, // waiting for the auto-update delay to expire
                         // (user is holding down button for auto-inc/dec)
  rate_timer_running_st, // waiting for the rate-timer to expire to know when to
                         // perform the auto inc/dec.
  rate_timer_expired_st, // when the rate-timer expires, perform the inc/dec
                         // function.
  add_second_to_clock_st // add a second to the clock time and reset the ms
                         // counter.
};
static enum clockControl_st_t currentState;

void clockControl_init() { currentState = init_st; }

// All printed messages for states are provided here.
#define INIT_ST_MSG "init state\n"
#define NEVER_TOUCHED_ST_MSG "never_touched_st\n"
#define WAITING_FOR_TOUCH_ST_MSG "waiting for touch_st\n"
#define AD_TIMER_RUNNING_ST_MSG "ad_timer_running_st\n"
#define AUTO_TIMER_RUNNING_ST_MSG "auto_timer_running_st\n"
#define RATE_TIMER_RUNNING_ST_MSG "rate_timer_running_st\n"
#define RATE_TIMER_EXPIRED_ST_MSG "rate_timer_expired_st\n"
// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint() {
  static enum clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case never_touched_st:
      printf(NEVER_TOUCHED_ST_MSG);
      break;
    case waiting_for_touch_st:
      printf(WAITING_FOR_TOUCH_ST_MSG);
      break;
    case adc_timer_running_st:
      printf(AD_TIMER_RUNNING_ST_MSG);
      break;
    case auto_timer_running_st:
      printf(AUTO_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_running_st:
      printf(RATE_TIMER_RUNNING_ST_MSG);
      break;
    case rate_timer_expired_st:
      printf(RATE_TIMER_EXPIRED_ST_MSG);
      break;
    }
  }
}
void clockControl_tick() {
  static uint16_t adcCounter = 0;
  static uint16_t autoCounter = 0;
  static uint16_t rateCounter = 0;
  static uint16_t secCounter = 0;

  debugStatePrint();

  // Perform state update first.
  switch (currentState) {
  case init_st:
    currentState = never_touched_st;
    break;
  case never_touched_st:
    if (display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;
  case waiting_for_touch_st:
    if (display_isTouched()) {
      display_clearOldTouchData();
      secCounter = 0;
      currentState = adc_timer_running_st;
    }
    if (secCounter == TICKS_PER_SECOND) {
      clockDisplay_advanceTimeOneSecond();
      secCounter = 0;
    }
    break;
  case adc_timer_running_st:
    if (display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE) {
      currentState = auto_timer_running_st;
    } else if (!display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  case auto_timer_running_st:
    if (display_isTouched() && autoCounter == AUTO_COUNTER_MAX_VALUE) {
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    } else if (!display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = waiting_for_touch_st;
    }
    break;
  case rate_timer_running_st:
    if (display_isTouched() && rateCounter == RATE_COUNTER_MAX_VALUE) {
      currentState = rate_timer_expired_st;
    } else if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;
  case rate_timer_expired_st:
    if (display_isTouched()) {
      clockDisplay_performIncDec();
      currentState = rate_timer_running_st;
    } else if (!display_isTouched()) {
      currentState = waiting_for_touch_st;
    }
    break;
  default:
    printf("State ERROR {currentState:%d}", currentState);
    break;
  }

  // Perform state action next.
  switch (currentState) {
  case init_st:
    break;
  case never_touched_st:
    break;
  case waiting_for_touch_st:
    adcCounter = 0;
    autoCounter = 0;
    rateCounter = 0;
    secCounter++;
    break;
  case adc_timer_running_st:
    adcCounter++;
    break;
  case auto_timer_running_st:
    autoCounter++;
    break;
  case rate_timer_running_st:
    rateCounter++;
    break;
  case rate_timer_expired_st:
    rateCounter = 0;
    break;
  default:
    printf("Action ERROR {currentState:%d}", currentState);
    break;
  }
}
