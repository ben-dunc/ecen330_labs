/*
    intervalTimer.c
    Made by Benjamin Duncan
    ECEn 330 - BYU Fall Semester 2021
*/

#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

#define TIMER_0_GPIO_DEVICE_BASE_ADDRESS XPAR_AXI_TIMER_0_BASEADDR
#define TIMER_1_GPIO_DEVICE_BASE_ADDRESS XPAR_AXI_TIMER_1_BASEADDR
#define TIMER_2_GPIO_DEVICE_BASE_ADDRESS XPAR_AXI_TIMER_2_BASEADDR

#define TIMER_CLOCK_FREQ_HZ XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;

#define TCSR0_OFFSET 0x00
#define TLR0_OFFSET 0x04
#define TCR0_OFFSET 0x08
#define TCSR1_OFFSET 0x10
#define TLR1_OFFSET 0x14
#define TCR1_OFFSET 0x18

#define CASC_MASK 0x0800
#define UTD_MASK 0x0002
#define LOAD_MASK 0x0020
#define ENT_MASK 0x0080

#define SHIFT_32 32

#define CLEAR 0

// A helper function to read the GPIO register
int32_t timer_readGpioRegister(int32_t offset, uint32_t timerNumber) {
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    return Xil_In32(TIMER_0_GPIO_DEVICE_BASE_ADDRESS + offset);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    return Xil_In32(TIMER_1_GPIO_DEVICE_BASE_ADDRESS + offset);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    return Xil_In32(TIMER_2_GPIO_DEVICE_BASE_ADDRESS + offset);
  } else {
    return CLEAR;
  }
}

// A helper function to write the GPIO register
void timer_writeGpioRegister(int32_t offset, int32_t value,
                             uint32_t timerNumber) {
  if (timerNumber == INTERVAL_TIMER_TIMER_0) {
    Xil_Out32(TIMER_0_GPIO_DEVICE_BASE_ADDRESS + offset, value);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_1) {
    Xil_Out32(TIMER_1_GPIO_DEVICE_BASE_ADDRESS + offset, value);
  } else if (timerNumber == INTERVAL_TIMER_TIMER_2) {
    Xil_Out32(TIMER_2_GPIO_DEVICE_BASE_ADDRESS + offset, value);
  }
}

void checkTimer(int32_t timerNumber) {
  if (timerNumber < INTERVAL_TIMER_TIMER_0 ||
      timerNumber > INTERVAL_TIMER_TIMER_2) {
    printf("%sERROR - timer %d doesn't exist.\n",
           "Please choose only timers 0, 1, or 2.\n", timerNumber);
  }
}

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
  checkTimer(timerNumber);
  timer_writeGpioRegister(TCSR0_OFFSET, CLEAR, timerNumber);
  timer_writeGpioRegister(TCSR1_OFFSET, CLEAR, timerNumber);
  timer_writeGpioRegister(TCSR0_OFFSET, CASC_MASK, timerNumber);

  if (timer_readGpioRegister(TCSR0_OFFSET, timerNumber) == CASC_MASK) {
    return INTERVAL_TIMER_STATUS_OK;
  } else {
    return INTERVAL_TIMER_STATUS_FAIL;
  }
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll() {
  int32_t timer1Status = intervalTimer_init(INTERVAL_TIMER_TIMER_0);
  int32_t timer2Status = intervalTimer_init(INTERVAL_TIMER_TIMER_1);
  int32_t timer3Status = intervalTimer_init(INTERVAL_TIMER_TIMER_2);

  if (INTERVAL_TIMER_STATUS_OK == timer1Status & timer2Status & timer3Status) {
    return INTERVAL_TIMER_STATUS_OK;
  } else {
    return INTERVAL_TIMER_STATUS_FAIL;
  }
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
  checkTimer(timerNumber);
  int32_t currentTCSR0 = timer_readGpioRegister(TCSR0_OFFSET, timerNumber);
  timer_writeGpioRegister(TCSR0_OFFSET, currentTCSR0 | ENT_MASK, timerNumber);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
  checkTimer(timerNumber);
  int32_t currentTCSR0 = timer_readGpioRegister(TCSR0_OFFSET, timerNumber);
  timer_writeGpioRegister(TCSR0_OFFSET, currentTCSR0 & ~ENT_MASK, timerNumber);
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
  checkTimer(timerNumber);
  timer_writeGpioRegister(TLR0_OFFSET, CLEAR, timerNumber);
  int32_t currentTCSR0 = timer_readGpioRegister(TCSR0_OFFSET, timerNumber);
  timer_writeGpioRegister(TCSR0_OFFSET, currentTCSR0 | LOAD_MASK, timerNumber);

  timer_writeGpioRegister(TLR0_OFFSET, CLEAR, timerNumber);
  int32_t currentTCSR1 = timer_readGpioRegister(TCSR1_OFFSET, timerNumber);
  timer_writeGpioRegister(TCSR1_OFFSET, currentTCSR1 | LOAD_MASK, timerNumber);

  intervalTimer_init(timerNumber);
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll() {
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
  intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
}
// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll() {}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  checkTimer(timerNumber);

  int64_t TCR1 = timer_readGpioRegister(TCR1_OFFSET, timerNumber);
  int64_t TCR0 = timer_readGpioRegister(TCR0_OFFSET, timerNumber);
  int64_t TCR1New = timer_readGpioRegister(TCR1_OFFSET, timerNumber);

  while (TCR1 != TCR1New) {
    TCR1 = timer_readGpioRegister(TCR1_OFFSET, timerNumber);
    TCR0 = timer_readGpioRegister(TCR0_OFFSET, timerNumber);
    TCR1New = timer_readGpioRegister(TCR1_OFFSET, timerNumber);
  }

  double time = (TCR1 << SHIFT_32) | TCR0;
  time = (double)time / TIMER_CLOCK_FREQ_HZ;
  return time;
}
