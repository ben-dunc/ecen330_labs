#include "isr.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "transmitter.h"
#include "trigger.h"
#include "queue.h"
#include "interrupts.h"
#include <stdint.h>
#include <stdio.h>

#define ADC_BUFFER_SIZE 100000

typedef uint32_t isr_AdcValue_t; // Used to represent ADC values in the ADC buffer.

// isr provides the isr_function() where you will place functions that require
// accurate timing. A buffer for storing values from the Analog to Digital
// Converter (ADC) is implemented in isr.c Values are added to this buffer by
// the code in isr.c. Values are removed from this queue by code in detector.c

// This implements a dedicated circular buffer for storing values
// from the ADC until they are read and processed by detector().
// adcBuffer_t is similar to a queue.
typedef struct {
	uint32_t indexIn;   // New values go here.
	uint32_t indexOut;  // Pull old values from here.
	uint32_t elementCount; // Number of elements in the buffer.
	uint32_t data[ADC_BUFFER_SIZE];  // Values are stored here.
} adcBuffer_t;

adcBuffer_t myAdcBuffer;

// Initialize the adc buffer circular array.
void adcBufferInit() {
  myAdcBuffer.indexIn = 0;
  myAdcBuffer.indexOut = 0;
}

// Performs inits for anything in isr.c
void isr_init() {
  adcBufferInit();
  transmitter_init();
  trigger_init();
  hitLedTimer_init();
  lockoutTimer_init();
}

// This returns the number of values in the ADC buffer.
uint32_t isr_adcBufferElementCount() {
  // Determine if the indexIn has already wrapped around.
  if (myAdcBuffer.indexIn < myAdcBuffer.indexOut) {
    // return the difference with the buffer size added to the indexIn.
    return (myAdcBuffer.indexIn + ADC_BUFFER_SIZE - myAdcBuffer.indexOut);
  }
  // If indexIn is not less than indexOut, return the difference between the two.
  else return myAdcBuffer.indexIn - myAdcBuffer.indexOut;
}


// This adds data to the ADC queue.
void isr_addDataToAdcBuffer(uint32_t adcData) {
  // Check if the adc buffer is full
  if (isr_adcBufferElementCount() == ADC_BUFFER_SIZE) {
    printf("ADC buffer is full!\n");
  }
  // If the buffer isn't full, add in the given data.
  else {
    myAdcBuffer.data[myAdcBuffer.indexIn] = adcData;
    // Increment indexIn, and wrap around the array if needed.
    myAdcBuffer.indexIn = ++myAdcBuffer.indexIn % ADC_BUFFER_SIZE;
    myAdcBuffer.data[myAdcBuffer.indexIn] = adcData;
  }
}

// Removes a single item from the ADC buffer.
// Does not signal an error if the ADC buffer is currently empty
// Simply returns a default value of 0 if the buffer is currently empty.
uint32_t isr_removeDataFromAdcBuffer() {
  // Check if buffer is empty.
    uint32_t returnValue = myAdcBuffer.data[myAdcBuffer.indexOut];
    myAdcBuffer.indexOut = ++myAdcBuffer.indexOut % ADC_BUFFER_SIZE;
    return returnValue;
  }

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function() {
  // Read the most recent value from the ADC and put add to adc buffer.
  isr_addDataToAdcBuffer(interrupts_getAdcData());
  // Call state machine tick functions
  transmitter_tick();
  trigger_tick();
  hitLedTimer_tick();
  lockoutTimer_tick();
}
