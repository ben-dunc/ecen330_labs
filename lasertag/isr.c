#include "isr.h"
#include "hitLedTimer.h"
#include "lockoutTimer.h"
#include "transmitter.h"
#include "trigger.h"
#include "queue.h"
#include <stdint.h>

#define ADC_BUFFER_SIZE 100000

typedef uint32_t
    isr_AdcValue_t; // Used to represent ADC values in the ADC buffer.

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

// This function is invoked by the timer interrupt at 100 kHz.
void isr_function() {
  transmitter_tick();
  trigger_tick();
  hitLedTimer_tick();
  lockoutTimer_tick();
  // Put latest ADC value in adcBuffer
	// Call state machine tick functions
}

// Implemented as a fixed-size circular buffer.
// indexIn always points to an empty location (by definition).
// indexOut always points to the oldest element.
// Buffer is empty if indexIn == indexOut.
// Buffer is full if incremented indexIn == indexOut.
// This adds data to the ADC queue. Data are removed from this queue and used by
// the detector.
void isr_addDataToAdcBuffer(uint32_t adcData) {
  myAdcBuffer.data[myAdcBuffer.indexIn] = adcData;
  myAdcBuffer.indexIn = (myAdcBuffer.indexIn + 1) % ADC_BUFFER_SIZE;
}

// Removes a single item from the ADC buffer.
// Does not signal an error if the ADC buffer is currently empty
// Simply returns a default value of 0 if the buffer is currently empty.
// This removes a value from the ADC buffer.
uint32_t isr_removeDataFromAdcBuffer() {

}

// This returns the number of values in the ADC buffer.
uint32_t isr_adcBufferElementCount() {

}