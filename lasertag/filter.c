/*************************************************
 *
 *  filter.c
 *
 *  This file is used to implement the functions provided
 *  in filter.h as per the instructions in milestone 3, task 1.
 *
 *  Benjamin Duncan and Kyle Cribbs
 *  BYU - ECEn 390 - Winter 2022
 *  Created Feb 3, 2021
 *
 ************************************************/

#include "filter.h"
#include "stdio.h"

// ============ HELPER FUNCTIONS ============

#define QUEUE_INIT_VALUE 0.0
#define IIR_A_COEFFICIENT_COUNT 10
#define IIR_B_COEFFICIENT_COUNT 11
#define FIR_FILTER_TAP_COUNT 81
#define Z_QUEUE_SIZE 10
#define OUTPUT_QUEUE_SIZE 2000
#define X_QUEUE_SIZE 81
#define Y_QUEUE_SIZE 11
#define FILTER_IIR_FILTER_COUNT 10
#define FILTER_FREQUENCY_COUNT 10

static queue_t zQueues[FILTER_IIR_FILTER_COUNT];
static queue_t outputQueues[FILTER_IIR_FILTER_COUNT];
static queue_t yQueue;
static queue_t xQueue;
static double currentPowerValue[FILTER_IIR_FILTER_COUNT];

static double oldestValue[FILTER_IIR_FILTER_COUNT] = {0};

const static double firCoefficients[FIR_FILTER_TAP_COUNT] = {
    4.3579622275120871e-04,  2.7155425450406482e-04,  6.3039002645024923e-05,
    -1.9349227837935689e-04, -4.9526428865281100e-04, -8.2651441681321251e-04,
    -1.1538970332472542e-03, -1.4254746936265958e-03, -1.5744703111426992e-03,
    -1.5281041447445794e-03, -1.2208092333090721e-03, -6.1008312441272067e-04,
    3.0761698758505478e-04,  1.4840192333212630e-03,  2.8123077568332011e-03,
    4.1290616416555957e-03,  5.2263464670258821e-03,  5.8739882867061607e-03,
    5.8504032099208122e-03,  4.9787419333799827e-03,  3.1637974805960073e-03,
    4.2435139609134468e-04,  -3.0844289197247119e-03, -7.0632027332701809e-03,
    -1.1078458037608581e-02, -1.4591395057493116e-02, -1.7004337345765962e-02,
    -1.7720830774014491e-02, -1.6213409845727576e-02, -1.2091458677988312e-02,
    -5.1609257765542604e-03, 4.5319860006883374e-03,  1.6679627700682660e-02,
    3.0718365411587241e-02,  4.5861875593064989e-02,  6.1160185621895735e-02,
    7.5579213982547147e-02,  8.8092930943210607e-02,  9.7778502396672379e-02,
    1.0390414346016497e-01,  1.0600000000000001e-01,  1.0390414346016497e-01,
    9.7778502396672379e-02,  8.8092930943210607e-02,  7.5579213982547147e-02,
    6.1160185621895735e-02,  4.5861875593064989e-02,  3.0718365411587241e-02,
    1.6679627700682660e-02,  4.5319860006883374e-03,  -5.1609257765542604e-03,
    -1.2091458677988312e-02, -1.6213409845727576e-02, -1.7720830774014491e-02,
    -1.7004337345765962e-02, -1.4591395057493116e-02, -1.1078458037608581e-02,
    -7.0632027332701809e-03, -3.0844289197247119e-03, 4.2435139609134468e-04,
    3.1637974805960073e-03,  4.9787419333799827e-03,  5.8504032099208122e-03,
    5.8739882867061607e-03,  5.2263464670258821e-03,  4.1290616416555957e-03,
    2.8123077568332011e-03,  1.4840192333212630e-03,  3.0761698758505478e-04,
    -6.1008312441272067e-04, -1.2208092333090721e-03, -1.5281041447445794e-03,
    -1.5744703111426992e-03, -1.4254746936265958e-03, -1.1538970332472542e-03,
    -8.2651441681321251e-04, -4.9526428865281100e-04, -1.9349227837935689e-04,
    6.3039002645024923e-05,  2.7155425450406482e-04,  4.3579622275120871e-04};

const static double iirACoefficientConstants
    [FILTER_FREQUENCY_COUNT][IIR_A_COEFFICIENT_COUNT] = {
        {-5.9637727070164033e+00, 1.9125339333078266e+01,
         -4.0341474540744230e+01, 6.1537466875368942e+01,
         -7.0019717951472359e+01, 6.0298814235239050e+01,
         -3.8733792862566432e+01, 1.7993533279581129e+01,
         -5.4979061224867900e+00, 9.0332828533800036e-01},
        {-4.6377947119071443e+00, 1.3502215749461573e+01,
         -2.6155952405269751e+01, 3.8589668330738334e+01,
         -4.3038990303252618e+01, 3.7812927599537112e+01,
         -2.5113598088113768e+01, 1.2703182701888078e+01,
         -4.2755083391143458e+00, 9.0332828533800102e-01},
        {-3.0591317915750929e+00, 8.6417489609637510e+00,
         -1.4278790253808840e+01, 2.1302268283304301e+01,
         -2.2193853972079225e+01, 2.0873499791105438e+01,
         -1.3709764520609394e+01, 8.1303553577931709e+00,
         -2.8201643879900526e+00, 9.0332828533800114e-01},
        {-1.4071749185996758e+00, 5.6904141470697542e+00,
         -5.7374718273676324e+00, 1.1958028362868898e+01,
         -8.5435280598354613e+00, 1.1717345583835954e+01,
         -5.5088290876998611e+00, 5.3536787286077576e+00,
         -1.2972519209655577e+00, 9.0332828533799836e-01},
        {8.2010906117760429e-01, 5.1673756579268675e+00, 3.2580350909221010e+00,
         1.0392903763919218e+01, 4.8101776408669252e+00, 1.0183724507092546e+01,
         3.1282000712126901e+00, 4.8615933365572221e+00, 7.5604535083145363e-01,
         9.0332828533800569e-01},
        {2.7080869856154477e+00, 7.8319071217995528e+00, 1.2201607990980708e+01,
         1.8651500443681559e+01, 1.8758157568004471e+01, 1.8276088095998947e+01,
         1.1715361303018838e+01, 7.3684394621253126e+00, 2.4965418284511749e+00,
         9.0332828533799858e-01},
        {4.9479835250075901e+00, 1.4691607003177602e+01, 2.9082414772101060e+01,
         4.3179839108869331e+01, 4.8440791644688872e+01, 4.2310703962394328e+01,
         2.7923434247706425e+01, 1.3822186510471004e+01, 4.5614664160654339e+00,
         9.0332828533799869e-01},
        {6.1701893352279811e+00, 2.0127225876810304e+01, 4.2974193398071584e+01,
         6.5958045321253252e+01, 7.5230437667866312e+01, 6.4630411355739554e+01,
         4.1261591079243900e+01, 1.8936128791950424e+01, 5.6881982915179901e+00,
         9.0332828533799114e-01},
        {7.4092912870072389e+00, 2.6857944460290138e+01, 6.1578787811202268e+01,
         9.8258255839887383e+01, 1.1359460153696310e+02, 9.6280452143026224e+01,
         5.9124742025776499e+01, 2.5268527576524267e+01, 6.8305064480743294e+00,
         9.0332828533800336e-01},
        {8.5743055776347745e+00, 3.4306584753117924e+01, 8.4035290411037209e+01,
         1.3928510844056848e+02, 1.6305115418161665e+02, 1.3648147221895832e+02,
         8.0686288623300072e+01, 3.2276361903872257e+01, 7.9045143816245078e+00,
         9.0332828533800080e-01}};

const static double
    iirBCoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_B_COEFFICIENT_COUNT] =
        {{9.0928731266278516e-10, -0.0000000000000000e+00,
          -4.5464365633139253e-09, -0.0000000000000000e+00,
          9.0928731266278505e-09, -0.0000000000000000e+00,
          -9.0928731266278505e-09, -0.0000000000000000e+00,
          4.5464365633139253e-09, -0.0000000000000000e+00,
          -9.0928731266278516e-10},
         {9.0928631323898375e-10, 0.0000000000000000e+00,
          -4.5464315661949193e-09, 0.0000000000000000e+00,
          9.0928631323898386e-09, 0.0000000000000000e+00,
          -9.0928631323898386e-09, 0.0000000000000000e+00,
          4.5464315661949193e-09, 0.0000000000000000e+00,
          -9.0928631323898375e-10},
         {9.0928660767801113e-10, 0.0000000000000000e+00,
          -4.5464330383900558e-09, 0.0000000000000000e+00,
          9.0928660767801115e-09, 0.0000000000000000e+00,
          -9.0928660767801115e-09, 0.0000000000000000e+00,
          4.5464330383900558e-09, 0.0000000000000000e+00,
          -9.0928660767801113e-10},
         {9.0928686475841070e-10, 0.0000000000000000e+00,
          -4.5464343237920538e-09, 0.0000000000000000e+00,
          9.0928686475841076e-09, 0.0000000000000000e+00,
          -9.0928686475841076e-09, 0.0000000000000000e+00,
          4.5464343237920538e-09, 0.0000000000000000e+00,
          -9.0928686475841070e-10},
         {9.0928655942373765e-10, 0.0000000000000000e+00,
          -4.5464327971186880e-09, 0.0000000000000000e+00,
          9.0928655942373761e-09, 0.0000000000000000e+00,
          -9.0928655942373761e-09, 0.0000000000000000e+00,
          4.5464327971186880e-09, 0.0000000000000000e+00,
          -9.0928655942373765e-10},
         {9.0928629381056898e-10, -0.0000000000000000e+00,
          -4.5464314690528456e-09, -0.0000000000000000e+00,
          9.0928629381056913e-09, -0.0000000000000000e+00,
          -9.0928629381056913e-09, -0.0000000000000000e+00,
          4.5464314690528456e-09, -0.0000000000000000e+00,
          -9.0928629381056898e-10},
         {9.0928394215999147e-10, -0.0000000000000000e+00,
          -4.5464197107999567e-09, -0.0000000000000000e+00,
          9.0928394215999134e-09, -0.0000000000000000e+00,
          -9.0928394215999134e-09, -0.0000000000000000e+00,
          4.5464197107999567e-09, -0.0000000000000000e+00,
          -9.0928394215999147e-10},
         {9.0929542362086042e-10, 0.0000000000000000e+00,
          -4.5464771181043024e-09, 0.0000000000000000e+00,
          9.0929542362086048e-09, 0.0000000000000000e+00,
          -9.0929542362086048e-09, 0.0000000000000000e+00,
          4.5464771181043024e-09, 0.0000000000000000e+00,
          -9.0929542362086042e-10},
         {9.0926195433045638e-10, 0.0000000000000000e+00,
          -4.5463097716522815e-09, 0.0000000000000000e+00,
          9.0926195433045630e-09, 0.0000000000000000e+00,
          -9.0926195433045630e-09, 0.0000000000000000e+00,
          4.5463097716522815e-09, 0.0000000000000000e+00,
          -9.0926195433045638e-10},
         {9.0910102251894290e-10, 0.0000000000000000e+00,
          -4.5455051125947149e-09, 0.0000000000000000e+00,
          9.0910102251894298e-09, 0.0000000000000000e+00,
          -9.0910102251894298e-09, 0.0000000000000000e+00,
          4.5455051125947149e-09, 0.0000000000000000e+00,
          -9.0910102251894290e-10}};

// init z queues - fills it with zeros
void initZQueues() {
  // Iterate through the z-queues to initialize each queue.
  for (uint32_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
    queue_init(&(zQueues[i]), Z_QUEUE_SIZE, "zQueue" + i);
    // fill the queue with zeros.
    for (uint32_t j = 0; j < Z_QUEUE_SIZE; j++)
      queue_overwritePush(&(zQueues[i]), QUEUE_INIT_VALUE);
  }
}

// init output queues - fills it with zeros
void initOutputQueues() {
  // Iterate through the output-queues to initialize each queue.
  for (uint32_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
    queue_init(&(outputQueues[i]), OUTPUT_QUEUE_SIZE, "outputQueue" + i);
    // fill the queue with zeros.
    for (uint32_t j = 0; j < OUTPUT_QUEUE_SIZE; j++)
      queue_overwritePush(&(outputQueues[i]), QUEUE_INIT_VALUE);
  }
}

// init x queue - fills it with zeros
void initXQueue() {
  // Initialize the x-queue.
  queue_init(&xQueue, X_QUEUE_SIZE, "Queue");
  // fill the queue with zeros.
  for (uint32_t i = 0; i < X_QUEUE_SIZE; i++)
    queue_overwritePush(&xQueue, QUEUE_INIT_VALUE);
}

// init y queue - fills it with zeros
void initYQueue() {
  // Initialize the y-queue.
  queue_init(&yQueue, Y_QUEUE_SIZE, "Queue");
  // fill the queue with zeros.
  for (uint32_t i = 0; i < Y_QUEUE_SIZE; i++)
    queue_overwritePush(&yQueue, QUEUE_INIT_VALUE);
}

// ============ GIVEN FUNCTIONS ============

// Must call this prior to using any filter functions.
// Initialized all of the queues.
void filter_init() {
  initXQueue();
  initYQueue();
  initZQueues();
  initOutputQueues();
}

// Use this to copy an input into the input queue of the FIR-filter (xQueue).
void filter_addNewInput(double x) { queue_overwritePush(&xQueue, x); }

// Fills a queue with the given fillValue. For example,
// if the queue is of size 10, and the fillValue = 1.0,
// after executing this function, the queue will contain 10 values
// all of them 1.0.
void filter_fillQueue(queue_t *q, double fillValue) {
  // fill the queue with zeros.
  for (uint32_t i = 0; i < queue_size(q); i++)
    queue_overwritePush(q, fillValue);
}

// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter() {
  double y = 0.0;
  // In each element of the xQueue,
  for (uint32_t i = 0; i < FIR_FILTER_TAP_COUNT; i++) {
    y += queue_readElementAt(&xQueue, FIR_FILTER_TAP_COUNT - 1 - i) *
         firCoefficients[i];
  }
  // Push that element into the yQueue.
  queue_overwritePush(&yQueue, y);
  // Return same value.
  return y;
}

// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueues[filterNumber].
double filter_iirFilter(uint16_t filterNumber) {
  double z_b = 0.0;
  double z_a = 0.0;
  double z = 0.0;
  // Do iir filtering
  // Sum up all of the yQueue elements multiplied by the iir b-coefficients.
  for (uint32_t i = 0; i < IIR_B_COEFFICIENT_COUNT; i++) {
    z_b += queue_readElementAt(&yQueue, IIR_B_COEFFICIENT_COUNT - 1 - i) *
           iirBCoefficientConstants[filterNumber][i];
  }
  // Sum up all of the zQueue elements multiplied by the iir a-coefficients.
  for (uint32_t i = 0; i < IIR_A_COEFFICIENT_COUNT; i++) {
    z_a += queue_readElementAt(&zQueues[filterNumber],
                               IIR_A_COEFFICIENT_COUNT - 1 - i) *
           iirACoefficientConstants[filterNumber][i];
  }
  // Subtract the separate b and a values.
  z = z_b - z_a;
  // Push the resulting value onto the zQueue.
  queue_overwritePush(&zQueues[filterNumber], z);
  queue_overwritePush(&outputQueues[filterNumber], z);
  return z;
}

// Use this to compute the power for values contained in an outputQueue.
// If force == true, then recompute power by using all values in the
// outputQueue. This option is necessary so that you can correctly compute power
// values the first time. After that, you can incrementally compute power values
// by:
// 1. Keeping track of the power computed in a previous run, call this
// prev-power.
// 2. Keeping track of the oldest outputQueue value used in a previous run, call
// this oldest-value.
// 3. Get the newest value from the power queue, call this newest-value.
// 4. Compute new power as: prev-power - (oldest-value * oldest-value) +
// (newest-value * newest-value). Note that this function will probably need an
// array to keep track of these values for each of the 10 output queues.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch,
                           bool debugPrint) {
  double newestValue =
      queue_readElementAt(&outputQueues[filterNumber], OUTPUT_QUEUE_SIZE - 1);
  double newPower = 0;
  // if force compute from scratch, compute the power.
  if (forceComputeFromScratch) {
    // Compute the power.
    for (uint32_t j = 0; j < OUTPUT_QUEUE_SIZE; j++) {
      newPower += queue_readElementAt(&outputQueues[filterNumber], j) *
                  queue_readElementAt(&outputQueues[filterNumber], j);
    }
  }
  // If the power does not need to be computed from scratch, use the previous
  // and newest power values to compute the new power.
  else {
    newPower = currentPowerValue[filterNumber] -
               (oldestValue[filterNumber] * oldestValue[filterNumber]) +
               (newestValue * newestValue);
  }
  // Assign the outputqueue element to the oldest value array.
  oldestValue[filterNumber] =
      queue_readElementAt(&outputQueues[filterNumber], 0);
  currentPowerValue[filterNumber] = newPower;

  return newPower;
}

// Returns the last-computed output power value for the IIR filter
// [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber) {
  return currentPowerValue[filterNumber];
}

// Get a copy of the current power values.
// This function copies the already computed values into a previously-declared
// array so that they can be accessed from outside the filter software by the
// detector. Remember that when you pass an array into a C function, changes to
// the array within that function are reflected in the returned array.
void filter_getCurrentPowerValues(double powerValues[]) {
  // Assign the power value array as the current power value array.
  for (uint8_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
    powerValues[i] = currentPowerValue[i];
  }
}

// Using the previously-computed power values that are current stored in
// currentPowerValue[] array, Copy these values into the normalizedArray[]
// argument and then normalize them by dividing all of the values in
// normalizedArray by the maximum power value contained in currentPowerValue[].
void filter_getNormalizedPowerValues(double normalizedArray[],
                                     uint16_t *indexOfMaxValue) {
  double maxValue = currentPowerValue[0];
  // Iterate through the current power array and find the index of the maximum
  // power value.
  for (uint16_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
    if (currentPowerValue[i] > maxValue) {
      maxValue = currentPowerValue[i];
      *indexOfMaxValue = i;
    }
  }
  // Iterate through the power array and normalize each value by dividing it by
  // the maximum power value.
  for (uint16_t i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
    normalizedArray[i] =
        currentPowerValue[i] / currentPowerValue[*indexOfMaxValue];
  }
}

// Returns the array of FIR coefficients.
const double *filter_getFirCoefficientArray() { return firCoefficients; }

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount() { return FIR_FILTER_TAP_COUNT; }

// Returns the array of coefficients for a particular filter number.
const double *filter_getIirACoefficientArray(uint16_t filterNumber) {
  return iirACoefficientConstants[filterNumber];
}

// Returns the number of A coefficients.
uint32_t filter_getIirACoefficientCount() { return IIR_A_COEFFICIENT_COUNT; }

// Returns the array of coefficients for a particular filter number.
const double *filter_getIirBCoefficientArray(uint16_t filterNumber) {
  return iirBCoefficientConstants[filterNumber];
}

// Returns the number of B coefficients.
uint32_t filter_getIirBCoefficientCount() { return IIR_B_COEFFICIENT_COUNT; }

// Returns the size of the yQueue.
uint32_t filter_getYQueueSize() { return queue_size(&yQueue); }

// Returns the decimation value.
uint16_t filter_getDecimationValue() {
  return 0; // TODO!!! WE HAVEN'T DONE THIS FUNCTION YET!
}

// Returns the address of xQueue.
queue_t *filter_getXQueue() { return &xQueue; }

// Returns the address of yQueue.
queue_t *filter_getYQueue() { return &yQueue; }

// Returns the address of zQueue for a specific filter number.
queue_t *filter_getZQueue(uint16_t filterNumber) {
  return &zQueues[filterNumber];
}

// Returns the address of the IIR output-queue for a specific filter-number.
queue_t *filter_getIirOutputQueue(uint16_t filterNumber) {
  return &outputQueues[filterNumber];
}

// Prints a celebratory message.
void filter_runTest() {
  printf("IT WORKS!!! MWHAHAHAHAHAH!!!! UNLIMITED POWER!");
}