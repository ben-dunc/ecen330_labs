// Created by Benjamin Duncan - Dec 2, 2021

#include <stdbool.h>
#include <stdint.h>

#ifndef GLOBALS_H
#define GLOBALS_H

#define DIFFICULTY_EASY 0
#define DIFFICULTY_MEDIUM 1
#define DIFFICULTY_HARD 2

#define PLAYER_WON 1
#define COMPUTER_WON 0

// set difficulty
void globals_setDifficulty(uint8_t diff);
// set difficulty
uint8_t globals_getDifficulty();

// set if player won or lost
void globals_setPlayerWon(bool pWon);
// set if player won or lost
bool globals_getPlayerWon();

// set if player won or lost
void globals_setVerses(bool v);
// set if player won or lost
bool globals_getVerses();

#endif /* GLOBALS_H */