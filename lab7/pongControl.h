// Created by Benjamin Duncan - Dec 2, 2021

#ifndef PONGCONTROL_H
#define PONGCONTROL_H

/*
    To do:
    1. DONE! Create init and tick functions
    2. Create screen state machine skeleton
    3. Determine how big each "pixel" is going to be
    3. Create start screen and gameover screen
*/

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