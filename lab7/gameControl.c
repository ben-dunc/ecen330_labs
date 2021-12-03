// Created by Benjamin Duncan - Dec 2, 2021

#include <stdio.h>
#include "gameControl.h"

// global variables
bool isComplete = false;
bool isEnabled = false;

// functions

// inits the game state function
void gameControl_init() {

}

// ticks the game state function
void gameControl_tick() {

}

// sets the enabled flag high
void gameControl_enable() {
    isEnabled = true;
}

// sets the enabled flag low
void gameControl_disable() {
    isEnabled = false;
}

// returns if the state machine is complete
bool gameControl_isComplete() {
    return isComplete;
}