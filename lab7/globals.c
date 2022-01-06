// Created by Benjamin Duncan - Dec 2, 2021

#include "globals.h"
#include <stdbool.h>
#include <stdint.h>

static uint8_t difficulty = 1;
static bool playerWon = false;
static bool verses = false;

// set difficulty
void globals_setDifficulty(uint8_t diff) { difficulty = diff; }

// set difficulty
uint8_t globals_getDifficulty() { return difficulty; }

// set if player won or lost
void globals_setPlayerWon(bool pWon) { playerWon = pWon; }

// set if player won or lost
bool globals_getPlayerWon() { return playerWon; }

// set if player won or lost
void globals_setVerses(bool v) { verses = v; }

// set if player won or lost
bool globals_getVerses() { return verses; }