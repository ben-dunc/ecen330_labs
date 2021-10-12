/*
    switches.c
    Created by Benjamin Duncan
    for ECEN 330 at BYU on Sept 13, 2021
*/

#include "xil_io.h"
#include "xparameters.h"
#include <leds.h>
#include <stdio.h>
#include <switches.h>

#define SWITCHES_GPIO_DEVICE_BASE_ADDRESS XPAR_SLIDE_SWITCHES_BASEADDR
#define DATA_OFFSET 0x00
#define TRI_OFFSET 0x04
#define PINS_INPUT 0xF
#define PINS_OUTPUT 0x0
#define ALL_SWITCHES_HIGH 0xF

// A helper function to read the GPIO register
int32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset);
}

// A helper function to write the GPIO register
void switches_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(SWITCHES_GPIO_DEVICE_BASE_ADDRESS + offset, value);
}

// Initializes the SWITCHES driver software and hardware. Returns one of the
// STATUS values defined above.
int32_t switches_init() {
  // SET AS INPUT and check
  switches_writeGpioRegister(TRI_OFFSET, PINS_INPUT);
  int32_t reading = switches_readGpioRegister(TRI_OFFSET);

  bool LED_FAILURE;
  leds_init(LED_FAILURE);

  if (!LED_FAILURE && reading & SWITCHES_SW0_MASK &&
      reading & SWITCHES_SW1_MASK && reading & SWITCHES_SW2_MASK &&
      reading & SWITCHES_SW3_MASK) {
    return SWITCHES_INIT_STATUS_OK;
  } else {
    return SWITCHES_INIT_STATUS_FAIL;
  }
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read() {
  int32_t reading = switches_readGpioRegister(DATA_OFFSET);
  leds_write(reading);
  return reading;
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void switches_runTest() {
  if (switches_init() == SWITCHES_INIT_STATUS_FAIL) {
    printf(
        "SWITCHES init failure. This may be a result of missing hardware.\n");
  }

  while (switches_read() != ALL_SWITCHES_HIGH)
    ;
}