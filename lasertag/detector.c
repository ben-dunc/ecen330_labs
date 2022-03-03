
#include "detector.h"
#include "isr.h"
#include "queue.h"
#include "filter.h"
#include "lockoutTimer.h"
#include <stdbool.h>
#include <stdint.h>

#define ADC_MAX_VALUE 4095

typedef uint16_t detector_hitCount_t;

static uint8_t numNewFilterInput = 0;
volatile static bool hitDetected = false; 
static uint32_t hitFreqNum = 0;

static uint32_t detector_hitArray[FILTER_FREQUENCY_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool detector_ignoreArray[FILTER_FREQUENCY_COUNT];

// Always have to init things.
// bool array is indexed by frequency number, array location set for true to
// ignore, false otherwise. This way you can ignore multiple frequencies.
void detector_init(bool ignoredFrequencies[]) {
    // Initialize filter
    filter_init();
    // Copy the input ignored frequencies into the detector ingore array.
    detector_ignoreArray = ignoredFrequencies;
}

// Runs the entire detector: decimating fir-filter, iir-filters,
// power-computation, hit-detection. if interruptsCurrentlyEnabled = true, interrupts
// are running. If interruptsCurrentlyEnabled = false you can pop values from the
// ADC buffer without disabling interrupts. If interruptsCurrentlyEnabled = true,
// do the following:
// 1. disable interrupts.
// 2. pop the value from the ADC buffer.
// 3. re-enable interrupts.
// Ignore hits that are detected on the frequencies specified during detector_init().
// Your own frequency (based on the switches) is a good choice to ignore.
// Assumption: draining the ADC buffer occurs faster than it can fill.
void detector(bool interruptsCurrentlyEnabled) {
    uint32_t elementCount = isr_adcBufferElementCount();

    // loop elementCount
    for (int i = 0; i < elementCount; i++) {
        // if it's enabled, disable it.
        if (interruptsCurrentlyEnabled) {
            interrupts_disabledArmInts();
        }

        uint32_t rawAdcValue = isr_removeDataFromAdcBuffer();

        // if it was disabled, enable it. \_(o_o)_/
        if (interruptsCurrentlyEnabled) {
            interrupts_enableArmInts();
        }

        double scaledAdcValue = (((double) rawAdcValue) / (ADC_MAX_VALUE / 2)) - 1;

        filter_addNewInput(scaledAdcValue);
        numNewFilterInput++;

        // if filters have been called 10 times (decimation)
        if (numNewFilterInput == 10) {
            numNewFilterInput = 0;
            
            // invoke FIR filter
            filter_firFilter();
            // invoke IIR filters
            for (int j = 0; j < FILTER_FREQUENCY_COUNT; j++) {
                filter_iirFilter(j);
            }
            // compute power for all 10 banks
            for (int j = 0; j < FILTER_FREQUENCY_COUNT; j++) {
                filter_computePower(j, false, false);
            }

            // if lockout timer running
            if (!lockoutTimer_running()) {
                // run hit detection algorithm
                // if: detected hit and frequency is not ignored
                // then do the rest...

                // TODO
            }
        }
    }
}

// Returns true if a hit was detected.
bool detector_hitDetected() {
    return hitDetected;
}

// Returns the frequency number that caused the hit.
uint16_t detector_getFrequencyNumberOfLastHit() {
    return hitFreqNum;
}

// Clear the detected hit once you have accounted for it.
void detector_clearHit() {
    hitDetected = false;
    hitFreqNum = 0;
}

// Ignore all hits. Used to provide some limited invincibility in some game
// modes. The detector will ignore all hits if the flag is true, otherwise will
// respond to hits normally.
void detector_ignoreAllHits(bool flagValue) {
    // do we need to do something here yet?
}

// Get the current hit counts.
// Copy the current hit counts into the user-provided hitArray
// using a for-loop.
void detector_getHitCounts(detector_hitCount_t hitArray[]) {
    // loop through frequencies and copy
    for (int i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
        hitArray[i] = detector_hitArray[i];
    }
}

// Allows the fudge-factor index to be set externally from the detector.
// The actual values for fudge-factors is stored in an array found in detector.c
void detector_setFudgeFactorIndex(uint32_t factor) {

}

// Encapsulate ADC scaling for easier testing.
double detector_getScaledAdcValue(isr_AdcValue_t adcValue) {
    return scaledAdcValue = (((double) adcValue) / (ADC_MAX_VALUE / 2)) - 1;
}

/*******************************************************
 ****************** Test Routines **********************
 ******************************************************/

// Students implement this as part of Milestone 3, Task 3.
void detector_runTest() {
    printf("IT WORKS!!!!! MWHAHAHAHAHAHAHAHAH!!!!!!!\n\t\\_(o_o)_/ \\_(o_o)_/ \\_(o_o)_/ \\_(o_o)_/ \\_(o_o)_/");
}
