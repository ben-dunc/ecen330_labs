/*
    buttons.c
    Created by Benjamin Duncan
    for ECEN 330 at BYU on Sept 13, 2021
*/

#include "xil_io.h"
#include "xparameters.h"
#include <buttons.h>
#include <display.h>
#include <stdio.h>

#define BUTTONS_GPIO_DEVICE_BASE_ADDRESS XPAR_PUSH_BUTTONS_BASEADDR
#define DATA_OFFSET 0x00
#define TRI_OFFSET 0x04
#define PINS_INPUT 0xF
#define PINS_OUTPUT 0x0
#define ALL_BUTTONS_HIGH 0x000F

#define X_ORIGIN 0
#define Y_ORIGIN 0
#define BOX_WIDTH DISPLAY_WIDTH / 4
#define BOX_HEIGHT DISPLAY_HEIGHT / 2
#define TEXT_Y BOX_HEIGHT / 2
#define TEXT_X BOX_WIDTH / 5
#define BTN0 "BTN0"
#define BTN1 "BTN1"
#define BTN2 "BTN2"
#define BTN3 "BTN3"
#define TEXT_SIZE 2

int32_t buttonsPushed = 0x0;

// A helper function to read the GPIO register
int32_t buttons_readGpioRegister(int32_t offset) {
  return Xil_In32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset);
}

// A helper function to write the GPIO register
void buttons_writeGpioRegister(int32_t offset, int32_t value) {
  Xil_Out32(BUTTONS_GPIO_DEVICE_BASE_ADDRESS + offset, value);
}

// Initializes the button driver software and hardware. Returns one of the
// defined status values (above).
int32_t buttons_init() {

  // SET AS INPUT and check
  display_init();
  display_fillRect(X_ORIGIN, Y_ORIGIN, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                   DISPLAY_BLACK);
  display_setTextSize(TEXT_SIZE);
  buttons_writeGpioRegister(TRI_OFFSET, PINS_INPUT);
  int32_t reading = buttons_readGpioRegister(TRI_OFFSET);

  if (reading & BUTTONS_BTN0_MASK && reading & BUTTONS_BTN1_MASK &&
      reading & BUTTONS_BTN2_MASK && reading & BUTTONS_BTN3_MASK) {
    return BUTTONS_INIT_STATUS_OK;
  } else {
    return BUTTONS_INIT_STATUS_FAIL;
  }
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() {
  int32_t reading = buttons_readGpioRegister(DATA_OFFSET);

  // BTN 0
  if (reading & BUTTONS_BTN0_MASK && !(buttonsPushed & BUTTONS_BTN0_MASK)) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed | BUTTONS_BTN0_MASK;
  } else if (!(reading & BUTTONS_BTN0_MASK) &&
             buttonsPushed & BUTTONS_BTN0_MASK) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed & ~BUTTONS_BTN0_MASK;
  }

  // BTN 1
  if (reading & BUTTONS_BTN1_MASK && !(buttonsPushed & BUTTONS_BTN1_MASK)) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed | BUTTONS_BTN1_MASK;
  } else if (!(reading & BUTTONS_BTN1_MASK) &&
             buttonsPushed & BUTTONS_BTN1_MASK) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed & ~BUTTONS_BTN1_MASK;
  }

  // BTN 2
  if (reading & BUTTONS_BTN2_MASK && !(buttonsPushed & BUTTONS_BTN2_MASK)) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed | BUTTONS_BTN2_MASK;
  } else if (!(reading & BUTTONS_BTN2_MASK) &&
             buttonsPushed & BUTTONS_BTN2_MASK) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed & ~BUTTONS_BTN2_MASK;
  }

  // BTN 3
  if (reading & BUTTONS_BTN3_MASK && !(buttonsPushed & BUTTONS_BTN3_MASK)) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed | BUTTONS_BTN3_MASK;
  } else if (!(reading & BUTTONS_BTN3_MASK) &&
             buttonsPushed & BUTTONS_BTN3_MASK) {
    // set buttonsPushed
    buttonsPushed = buttonsPushed & ~BUTTONS_BTN3_MASK;
  }

  return reading;
}

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest() {
  if (buttons_init() == BUTTONS_INIT_STATUS_FAIL) {
    printf("BUTTONS init failure. This may be a result of missing failure.\n");
  }

  while (buttons_read() != ALL_BUTTONS_HIGH) {
    int32_t reading = buttons_read();

    // BTN 0
    if (reading & BUTTONS_BTN0_MASK && !(buttonsPushed & BUTTONS_BTN0_MASK)) {
      // draw box
      display_fillRect(BOX_WIDTH + BOX_WIDTH + BOX_WIDTH, Y_ORIGIN, BOX_WIDTH,
                       BOX_HEIGHT, DISPLAY_YELLOW);
      display_setCursor(BOX_WIDTH + BOX_WIDTH + BOX_WIDTH + TEXT_X, TEXT_Y);
      display_setTextColor(DISPLAY_BLACK);
      display_println(BTN0);
    } else if (!(reading & BUTTONS_BTN0_MASK) &&
               buttonsPushed & BUTTONS_BTN0_MASK) {
      // erase box
      display_fillRect(BOX_WIDTH + BOX_WIDTH + BOX_WIDTH, Y_ORIGIN, BOX_WIDTH,
                       BOX_HEIGHT, DISPLAY_BLACK);
    }

    // BTN 1
    if (reading & BUTTONS_BTN1_MASK && !(buttonsPushed & BUTTONS_BTN1_MASK)) {
      // draw box
      display_fillRect(BOX_WIDTH + BOX_WIDTH, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT,
                       DISPLAY_GREEN);
      display_setCursor(BOX_WIDTH + BOX_WIDTH + TEXT_X, TEXT_Y);
      display_setTextColor(DISPLAY_BLACK);
      display_println(BTN1);
    } else if (!(reading & BUTTONS_BTN1_MASK) &&
               buttonsPushed & BUTTONS_BTN1_MASK) {
      // erase box
      display_fillRect(BOX_WIDTH + BOX_WIDTH, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT,
                       DISPLAY_BLACK);
    }

    // BTN 2
    if (reading & BUTTONS_BTN2_MASK && !(buttonsPushed & BUTTONS_BTN2_MASK)) {
      // draw box
      display_fillRect(BOX_WIDTH, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT, DISPLAY_RED);
      display_setCursor(BOX_WIDTH + TEXT_X, TEXT_Y);
      display_setTextColor(DISPLAY_WHITE);
      display_println(BTN2);
    } else if (!(reading & BUTTONS_BTN2_MASK) &&
               buttonsPushed & BUTTONS_BTN2_MASK) {
      // erase box
      display_fillRect(BOX_WIDTH, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT,
                       DISPLAY_BLACK);
    }

    // BTN 3
    if (reading & BUTTONS_BTN3_MASK && !(buttonsPushed & BUTTONS_BTN3_MASK)) {
      // draw box
      display_fillRect(X_ORIGIN, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT, DISPLAY_BLUE);
      display_setCursor(TEXT_X, TEXT_Y);
      display_setTextColor(DISPLAY_WHITE);
      display_println(BTN3);
    } else if (!(reading & BUTTONS_BTN3_MASK) &&
               buttonsPushed & BUTTONS_BTN3_MASK) {
      // erase box
      display_fillRect(X_ORIGIN, Y_ORIGIN, BOX_WIDTH, BOX_HEIGHT,
                       DISPLAY_BLACK);
    }
  }
}