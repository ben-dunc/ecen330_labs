// Created by Benjamin Duncan - Dec 2, 2021

#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <stdbool.h>

// inits the game state function
void gameControl_init();
// ticks the game state function
void gameControl_tick();
// sets the enabled flag high
void gameControl_enable();
// sets the enabled flag low
void gameControl_disable();
// returns if the state machine is complete
bool gameControl_isComplete();

#endif /* GAMECONTROL_H */
