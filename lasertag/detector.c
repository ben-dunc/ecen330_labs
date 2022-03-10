
#include "detector.h"
#include "isr.h"
#include "queue.h"
#include "filter.h"
#include "lockoutTimer.h"
#include "hitLedTimer.h"
#include "runningModes.h"
#include "buttons.h"
#include "interrupts.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define ADC_MAX_VALUE 4095
#define FUDGE_FACTOR 1000
#define TEST_POWER_ARRAY_SIZE 12

typedef uint16_t detector_hitCount_t;

static uint8_t numNewFilterInput = 0;
volatile static bool hitDetected = false;
static uint32_t hitFreqNum = 0;

static uint32_t detector_hitArray[FILTER_FREQUENCY_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t detector_ignoreArray[FILTER_FREQUENCY_COUNT];
static double powerValues[FILTER_FREQUENCY_COUNT];

// test values
volatile static bool testFlag = false; 
static double testPowerData[TEST_POWER_ARRAY_SIZE][FILTER_FREQUENCY_COUNT] = {
    {1000, 0.1, 0.1, 0.1, 3, 0.1, 0.1, 0.1, 0.1, 0.1},
    {0.1, 1000, 0.1, 0.1, 0.1, 3, 0.5, 0.1, 0.1, 0.1},
    {0.1, 0.1, 1000, 0.1, 0.1, 0.5, 0.1, 0.1, 0.1, 0.1},
    {0.1, 0.1, 0.1, 1000, 0.1, 0.1, 0.1, 0.1, 0.5, 0.1},
    {0.5, 0.1, 3, 0.1, 1000, 0.1, 0.1, 0.1, 0.1, 0.1},
    {0.1, 0.1, 0.1, 0.1, 0.1, 1000, 0.1, 0.1, 0.1, 0.1},
    {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 1000, 0.1, 0.1, 0.1},
    {0.1, 0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 1000, 0.1, 0.1},
    {0.1, 0.1, 0.1, 0.1, 0.1, 3, 0.1, 0.1, 1000, 0.1},
    {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 1000},
    {0.1, 101, 0.1, 0.5, 0.1, 1, 0.1, 0.1, 0, 0.1},
    {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}
};

// Always have to init things.
// bool array is indexed by frequency number, array location set for true to
// ignore, false otherwise. This way you can ignore multiple frequencies.
void detector_init(bool ignoredFrequencies[]) {
    // Initialize filter
    filter_init();
    // Copy the input ignored frequencies into the detector ingore array.
    for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
        detector_ignoreArray[i] = ignoredFrequencies[i];
    }
}

// Performs the hit detection algorithm to find the player who hit you.
void detector_hitDetectionAlgorithm(double values[]) {
    uint8_t filterNum[FILTER_FREQUENCY_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    /*
    printf("\tunsorted: ");
    for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
        printf("%f ", values[i]);
    }
    printf("\n");
    */

    int32_t i, j;
    int8_t filter_temp;
    double temp;
    // sort the data
    for (i = 1; i < FILTER_FREQUENCY_COUNT; i++) {
        temp = values[i];
        j = i - 1;
        filter_temp = filterNum[i];
        // Swap the elements if element at j is less than element at j+1
        while (j >= 0 && values[j] > temp) {
            values[j + 1]  = values[j];
            filterNum[j + 1] = filterNum[j];
            j--;
        }
        values[j + 1] = temp;
        filterNum[j + 1] = filter_temp;
    }

    /*
    printf("\tsorted: ");
    //
    for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
        printf("%f ", values[i]);
    }
    printf("\n");

    printf("\tfilter numbers: ");
    //
    for (uint16_t i = 0; i < FILTER_FREQUENCY_COUNT; i++) {
        printf("%d ", filterNum[i]);
    }
    printf("\n");
    */

    // Select the median value
    uint32_t median = values[FILTER_FREQUENCY_COUNT / 2];
    // compute threshold.
    uint32_t threshold = median * FUDGE_FACTOR;
    // Find the maximum power
    uint32_t max_power = values[FILTER_FREQUENCY_COUNT - 1];
    //printf("\tMax power: %f\n", max_power);
    // See if the max power exceeds the threshold
    if (max_power > threshold) {
        // Its a hit!
        hitDetected = true;
        hitFreqNum = filterNum[FILTER_FREQUENCY_COUNT - 1];
    } else {
        hitDetected = false;
    }
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
    for (uint32_t i = 0; i < elementCount; i++) {
        // if it's enabled, disable it.
        if (interruptsCurrentlyEnabled) {
            interrupts_disableArmInts();
        }

        uint32_t rawAdcValue = isr_removeDataFromAdcBuffer();

        // if it was disabled, enable it. \_(o_o)_/
        if (interruptsCurrentlyEnabled) {
            interrupts_enableArmInts();
        }

        double scaledAdcValue = detector_getScaledAdcValue(rawAdcValue);

        filter_addNewInput(scaledAdcValue);
        numNewFilterInput++;

        // if filters have been called 10 times (decimation)
        if (numNewFilterInput == 10) {
            numNewFilterInput = 0;

            //printf("ADC buffer size: %d\n", isr_adcBufferElementCount());
            
            // invoke FIR filter
            filter_firFilter();
            //printf("ADC size after fir filter: %d\n", isr_adcBufferElementCount());
            // invoke IIR filters
            for (uint32_t j = 0; j < FILTER_FREQUENCY_COUNT; j++) {
                filter_iirFilter(j);
            }
            //printf("ADC size after iir filter: %d\n", isr_adcBufferElementCount());

            // compute power for all 10 banks
            for (uint32_t j = 0; j < FILTER_FREQUENCY_COUNT; j++) {
                filter_computePower(j, false, false);
            }
            //printf("ADC size after computing power: %d\n", isr_adcBufferElementCount());

            // if lockout timer running
            if (!lockoutTimer_running()) {
                // run hit detection algorithm
                filter_getCurrentPowerValues(powerValues);
                detector_hitDetectionAlgorithm(powerValues);
                
                // if: detected hit and frequency is not ignored
                if (hitDetected && !detector_ignoreArray[hitFreqNum]) {
                    lockoutTimer_start();
                    hitLedTimer_start();
                    detector_hitArray[hitFreqNum]++;
                }
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
    return (((double) adcValue) / (ADC_MAX_VALUE / 2)) - 1;
}

/*******************************************************
 ****************** Test Routines **********************
 ******************************************************/

// Students implement this as part of Milestone 3, Task 3.
void detector_runTest() {
    
    // run detector isolated test
    testFlag = true;

    printf("\n================= Running DETECTOR ISOLATED test =================\n\n");
    
    // iterate through test values
    for (uint16_t i = 0; i < TEST_POWER_ARRAY_SIZE; i++) {
        // print out values
        printf("Using the following values: ");
        // iterate through test values
        for (uint16_t j = 0; j < FILTER_FREQUENCY_COUNT - 1; j++) {
            printf("%f, ", testPowerData[i][j]);
        }
        printf("%f\n", testPowerData[i][FILTER_FREQUENCY_COUNT - 1]);

        // run detector function
        detector_hitDetectionAlgorithm(testPowerData[i]);

        // check if detected
        if (detector_hitDetected()) {
            printf("\t Hit detected at frequency: %d\n", hitFreqNum);
        } else {
            printf("\t NO hit detected.\n");   
        }
    }
    
    printf("\n====================================================================\n\n");

    // wait for user input for second test
    // printf("Press btn 1 to advance to the system demonstration test.\n");
    // // wait for button down
    // while (!(buttons_read() & BUTTONS_BTN0_MASK));
    // // wait for depress
    // while ((buttons_read() & BUTTONS_BTN0_MASK));

    // run system demonstration
    // printf("\n================ Running SYSTEM DEMONSTRATION test ================\n");
    // runningModes_continuous();
    // runningModes_shooter();
    // printf("\n====================================================================\n\n");

    printf("\n\nTHE END\n");
}
