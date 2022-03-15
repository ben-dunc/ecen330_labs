#include "transmitter.h"
#include "buttons.h"
#include "filter.h"
#include "mio.h"
#include "switches.h"
#include "utils.h"
#include <stdio.h>

// The transmitter state machine generates a square wave output at the chosen
// frequency as set by transmitter_setFrequencyNumber(). The step counts for the
// frequencies are provided in filter.h

#define TRANSMITTER_HIGH_VALUE 1
#define TRANSMITTER_LOW_VALUE 0
#define PULSE_WIDTH (testing ? 200 : TRANSMITTER_PULSE_WIDTH)
#define TRANSMITTER_TEST_TICK_PERIOD_IN_MS 50
#define NON_CONTINUOUS_TEST_DELAY 400
#define DIVIDE_BY 2

// state machine
typedef enum { init_st, high_st, low_st, finished_st } transmitter_st_t;
static transmitter_st_t currentState;

static uint16_t freqNum = 0;    // can be 0 - 9
static uint16_t newFreqNum = 0; // can be 0 - 9
volatile static bool transmitterRunning = false;
static bool continuousMode = false;
static uint16_t tickCount = 0;
static bool testing = false;

// Standard init function.
void transmitter_init() {
  // false disables any debug printing if there is a system failure during init.
  mio_init(false);
  buttons_init();  // Using buttons
  switches_init(); // and switches.

  // Configure the signal direction of the pin to be an output.
  mio_setPinAsOutput(TRANSMITTER_OUTPUT_PIN);
  // Initialize the state machine.
  currentState = init_st;
}

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber) {
  // set if it's in continuous mode
  if (continuousMode || !transmitterRunning) {
    freqNum = frequencyNumber;
    newFreqNum = freqNum;
  } else {
    newFreqNum = frequencyNumber;
  }
}

// Returns the current frequency setting.
uint16_t transmitter_getFrequencyNumber() { return freqNum; }

// Sets the mio pin high
void transmitter_set_jf1_to_one() {
  mio_writePin(TRANSMITTER_OUTPUT_PIN,
               TRANSMITTER_HIGH_VALUE); // Write a '1' to JF-1.
}

// Sets the mio pin low
void transmitter_set_jf1_to_zero() {
  mio_writePin(TRANSMITTER_OUTPUT_PIN,
               TRANSMITTER_LOW_VALUE); // Write a '1' to JF-1.
}

// Starts the transmitter.
void transmitter_run() {
  // update freqnum if necessary
  /*
  if (freqNum != newFreqNum) {
    transmitter_setFrequencyNumber(newFreqNum);
  }*/

  // Compute a safe number from the switches.
  uint16_t switchValue = switches_read() % FILTER_FREQUENCY_COUNT; 
  // set the frequency number based upon switch value.
  transmitter_setFrequencyNumber(switchValue); 


  // starting state machine
  currentState = init_st;
  transmitterRunning = true;
}

// Returns true if the transmitter is still running.
bool transmitter_running() { return transmitterRunning; }


// Turns on testing mode.
void transmitter_enableTestMode() { testing = true; }

// debugging output
void transmitter_debug() {
  static transmitter_st_t prevState = init_st;

  // Perform state updates.
  if (prevState != currentState) {
    // output states
    switch (prevState) {
    case init_st:
      printf("init_st\n");
      break;
    case high_st:
      printf("\nhigh_st\n");
      break;
    case low_st:
      printf("\nlow_st\n");
      break;
    case finished_st:
      printf("\nfinished_st\n");
      break;
    }

    prevState = currentState;
  }

  // output stuffs
  if (currentState == high_st) {
    printf("1");
    // output stuffs
  } else if (currentState == low_st) {
    printf("0");
  }

  printf("\t%d mod %d >= %d)\twith freqNum: %d\n", tickCount,
         filter_frequencyTickTable[freqNum],
         (filter_frequencyTickTable[freqNum] / DIVIDE_BY), freqNum);
}

// Standard tick function.
void transmitter_tick() {
  // call debug function
  if (testing) {
    transmitter_debug();
  }

  // Perform state updates.
  switch (currentState) {
  case init_st:
    // transition straight to transmit st
    currentState = high_st;
    tickCount = 0;
    transmitter_set_jf1_to_one();
    break;
  case high_st:
    // transition to finished_st when reached wavelength
    if (!continuousMode && (tickCount >= PULSE_WIDTH)) {
      currentState = finished_st;
      // Output zeros to the jf1 pin.
      transmitter_set_jf1_to_zero();
      transmitterRunning = false;
    }
    // transition to low
    else if (tickCount % filter_frequencyTickTable[freqNum] >=
             (filter_frequencyTickTable[freqNum] / DIVIDE_BY)) {
      transmitter_set_jf1_to_zero();
      currentState = low_st;
    }
    break;
  case low_st:
    // transition to finished_st when reached wavelength
    if (!continuousMode && (tickCount >= PULSE_WIDTH)) {
      currentState = finished_st;
      // Output zeros to the jf1 pin.
      transmitter_set_jf1_to_zero();
      transmitterRunning = false;
    }
    // transition to high
    else if (tickCount % filter_frequencyTickTable[freqNum] <
             filter_frequencyTickTable[freqNum] / DIVIDE_BY) {
      transmitter_set_jf1_to_one();
      currentState = high_st;
    }
    break;
    // finished state.
  case finished_st:
    // do nothing
    break;
  }

  // Perform state actions next.
  switch (currentState) {
  case init_st:
    // do nothing
    break;
  case high_st:
    // increment tick
    tickCount++;
    break;
  case low_st:
    // increment tick
    tickCount++;
    break;
  case finished_st:
    transmitterRunning = false;
    // do nothing
    break;
  }

  // printf("tick count:\t %d\n", tickCount);
}

// Tests the transmitter.
void transmitter_runTest() {
  transmitter_init();
  transmitter_enableTestMode();
  printf("starting transmitter_runTest()\n");
  // loop till button 1 is pressed
  while (!(buttons_read() &
           BUTTONS_BTN1_MASK)) { // Run continuously until BTN1 is pressed.
    printf("Select a frequency and push button 0 to start\n");
    // wait till button 0 is pressed
    while (!(buttons_read() & BUTTONS_BTN0_MASK))
      ;

    // Compute a safe number from the switches.
    uint16_t switchValue = switches_read() % FILTER_FREQUENCY_COUNT;
    // set the frequency number based upon switch value.
    transmitter_setFrequencyNumber(switchValue);
    printf("starting another test period switch frequency number\n and actual "
           "frequency number: %d, %d.\n",
           switchValue, transmitter_getFrequencyNumber());

    transmitter_run(); // Start the transmitter.

    // wait till done
    while (transmitter_running()) { // Keep ticking until it is done.
      transmitter_tick();           // tick.
      utils_msDelay(
          TRANSMITTER_TEST_TICK_PERIOD_IN_MS); // short delay between ticks.
    }

    printf("\ncompleted one test period.\n");
  }
  printf("Exited test.\n");
}

// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise,
// transmits one pulse-width and stops. To set continuous mode, you must invoke
// this function prior to calling transmitter_run(). If the transmitter is in
// currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false. It can stop immediately or wait
// until the last 200 ms pulse is complete. NOTE: while running continuously,
// the transmitter will change frequencies at the end of each 200 ms pulse.
void transmitter_setContinuousMode(bool continuousModeFlag) {
  continuousMode = continuousModeFlag;
}

// Tests the transmitter in non-continuous mode.
// The test runs until BTN1 is pressed.
// To perform the test, connect the oscilloscope probe
// to the transmitter and ground probes on the development board
// prior to running this test. You should see about a 300 ms dead
// spot between 200 ms pulses.
// Should change frequency in response to the slide switches.
void transmitter_runNoncontinuousTest() {
  transmitter_setContinuousMode(false);
  int16_t prevSwitchValue = switches_read() % FILTER_FREQUENCY_COUNT;

  printf("\n\n==========================================\nStarted NON-"
         "CONTINUOUS test. \nPress button 1 to finish the test. "
         "\nThe switches determine the frequency number.\n");

  // Run until button 1 is pressed
  while (!(buttons_read() & BUTTONS_BTN1_MASK)) {
    // set the frequency number based upon switch value.
    transmitter_setFrequencyNumber(switches_read() % FILTER_FREQUENCY_COUNT);
    transmitter_run();

    // Run till the transmitter is finished
    while (transmitter_running())
      ;
    utils_msDelay(NON_CONTINUOUS_TEST_DELAY);
  }
  // wait till depressed
  while ((buttons_read() & BUTTONS_BTN1_MASK))
    ;
  printf("Finished non continuous test.\n"
         "==========================================\n\n");
}

// Tests the transmitter in continuous mode.
// To perform the test, connect the oscilloscope probe
// to the transmitter and ground probes on the development board
// prior to running this test.
// Transmitter should continuously generate the proper waveform
// at the transmitter-probe pin and change frequencies
// in response to changes to the changes in the slide switches.
// Test runs until BTN1 is pressed.
void transmitter_runContinuousTest() {
  transmitter_init();
  transmitter_setContinuousMode(true);
  int16_t prevSwitchValue = switches_read() % FILTER_FREQUENCY_COUNT;

  printf("\n==========================================\nStarted CONTINUOUS "
         "test. \nPress button 1 to finish the test. \nThe "
         "switches determine the frequency number.\n");

  // Run until button 1 is pressed
  while (!(buttons_read() & BUTTONS_BTN1_MASK)) {
    transmitter_run();
    // Run till the transmitter is finished
    while (transmitter_running()) {
      uint16_t switchValue =
          switches_read() %
          FILTER_FREQUENCY_COUNT; // Compute a safe number from the switches.
      transmitter_setFrequencyNumber(
          switchValue); // set the frequency number based upon switch value.
    }
  }

  // wait till depressed
  while ((buttons_read() & BUTTONS_BTN1_MASK))
    ;

  printf("Finished continuous "
         "test.\n==========================================\n");
}
