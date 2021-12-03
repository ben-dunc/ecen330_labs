// Created by Benjamin Duncan - Dec 2, 2021

#include <stdio.h>
#include "pongControl.h"

bool isGameOver = false;
bool isEnabled = false;

// inits the pong control state machine
void pongControl_init() {
    
}

// ticks the pong control state machine
void pongControl_tick() {

}

// sets the enabled flag high
void pongControl_high() {
    isEnabled = true;
}

// sets the enabled flag low
void pongControl_disable() {
    isEnabled = false;
}

// returns if the state machine is complete
bool pongControl_isGameOver() {
    return isGameOver;
}