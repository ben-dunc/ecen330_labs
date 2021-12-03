// Created by Benjamin Duncan - Dec 2, 2021

#ifndef PONG_CONTROL_H
#define PONG_CONTROL_H

/*
    To do:
    1. Create init and tick functions
    2. Create screen state machine skeleton
    3. Create start screen and gameover screen
*/

void pongControl_init();
void pongControl_tick();
void pongControl_enable();
void pongControl_disable();
void pongControl_isGameOver();

#endif /* PONG_CONTROL_H */