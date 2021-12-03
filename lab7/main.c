// Created by Benjamin Duncan - Dec 2, 2021

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <xparameters.h>

#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "utils.h"

#include "gameControl.h"
#include "pongControl.h"

// Compute the timer clock freq.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
// Compute timer load value.
#define TIMER_LOAD_VALUE ((CONFIG_TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

#define INTERRUPTS_PER_SECOND (1.0 / CONFIG_TIMER_PERIOD)
#define INTERRUPTS_IN_PONG_TICK 5

#define TOTAL_SECONDS 120
#define MAX_INTERRUPT_COUNT (INTERRUPTS_PER_SECOND * TOTAL_SECONDS)

void initAll() {
  gameControl_init();
  pongControl_init();

  pongControl_enable();

  display_init();
  display_fillScreen(DISPLAY_BLACK);
}

// tick all
void tickAll() {
  gameControl_tick();
  pongControl_tick();
}

// All programs share the same main.
// Differences are limited to test_init() and isr_function().
int main() {
  printf("Running PONG.\n");
  printf("======================\n\n");
  initAll(); // Program specific.
  // Init all interrupts (but does not enable the interrupts at the devices).
  // Prints an error message if an internal failure occurs because the argument
  // = true.
  interrupts_initAll(true);
  interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
  interrupts_enableTimerGlobalInts();
  // Keep track of your personal interrupt count. Want to make sure that you
  // don't miss any interrupts.
  int32_t personalInterruptCount = 0;
  // Start the private ARM timer running.
  interrupts_startArmPrivateTimer();
  
  // Enable interrupts at the ARM.
  interrupts_enableArmInts();
  while (true) {
    if (interrupts_isrFlagGlobal) {
      // Count ticks.
      personalInterruptCount++;
      tickAll();
      interrupts_isrFlagGlobal = 0;

      if (personalInterruptCount >= MAX_INTERRUPT_COUNT)
        break;
      utils_sleep();
    }
  }

  interrupts_disableArmInts();
  printf("\n\n======================\n");
  printf("Terminating PONG.\n\n");
  printf("isr invocation count: %d\n", interrupts_isrInvocationCount());
  printf("internal interrupt count: %d\n", personalInterruptCount);
  return 0;
}

// Interrupt routine
void isr_function() {
  // Empty for flag method (flag set elsewhere)
}
