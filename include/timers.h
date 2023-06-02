#ifndef TIMERS_H
#define TIMERS_H 1
#include <stdint.h>
#include <variables.h>
extern volatile uint8_t pb_state;
extern volatile uint16_t timerticks;
extern volatile note NOTE;
extern volatile state GAME_STATE;
extern volatile uint16_t delay;
#endif