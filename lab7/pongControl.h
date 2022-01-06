// Created by Benjamin Duncan - Dec 2, 2021

#ifndef PONGCONTROL_H
#define PONGCONTROL_H

#include <stdbool.h>

// inits the pong control state machine
void pongControl_init();
// ticks the pong control state machine
void pongControl_tick();
// sets the enabled flag high
void pongControl_enable();
// sets the enabled flag low
void pongControl_disable();
// returns if the state machine is complete
bool pongControl_isGameOver();

#endif /* PONGCONTROL_H */