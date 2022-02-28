#include "isr.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "transmitter.h"
#include "trigger.h"
#include <stdint.h>

typedef uint32_t
    isr_AdcValue_t; // Used to represent ADC values in the ADC buffer.

// isr provides the isr_function() where you will place functions that require
// accurate timing. A buffer for storing values from the Analog to Digital
// Converter (ADC) is implemented in isr.c Values are added to this buffer by
// the code in isr.c. Values are removed from this queue by code in detector.c

// Performs inits for anything in isr.c
void isr_init() {
  transmitter_init();
  trigger_init();
  hitLedTimer_init();
  lockoutTimer_init();
}

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function() {
  transmitter_tick();
  trigger_tick();
  hitLedTimer_tick();
  lockoutTimer_tick();
}