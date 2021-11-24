// Created by Benjamin Duncan
// ECEn 330 - Nov 23, 2021

#include "globals.h"
#include <stdint.h>

static uint8_t globals_sequence[GLOBALS_MAX_FLASH_SEQUENCE];
static uint16_t globals_totalLength = 0;
static uint16_t globals_iterationLength = 0;

// This is the length of the complete sequence at maximum length.
// You must copy the contents of the sequence[] array into the global variable
// that you maintain. Do not just grab the pointer as this will fail.
void globals_setSequence(const uint8_t sequence[], uint16_t length) {
  // copy
  for (uint16_t i = 0; i < length; i++) {
    globals_sequence[i] = sequence[i];
  }
  globals_totalLength = length;
}

// This returns the value of the sequence at the index.
uint8_t globals_getSequenceValue(uint16_t index) {
  return globals_sequence[index];
}

// Retrieve the sequence length.
uint16_t globals_getSequenceLength() { return globals_totalLength; }

// This is the length of the sequence that you are currently working on.
void globals_setSequenceIterationLength(uint16_t length) {
  globals_iterationLength = length;
}

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the person playing the game works through the pattern one color at a time.
uint16_t globals_getSequenceIterationLength() {
  return globals_iterationLength;
}
