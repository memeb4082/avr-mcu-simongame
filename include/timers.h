#ifndef TIMERS_H
#define TIMERS_H 1
#include <stdint.h>
#include <states.h>
extern volatile uint8_t pb_state;
extern volatile uint16_t playback;
extern volatile uint16_t elapsed;
extern volatile uint16_t new_time;
extern volatile uint8_t allow_update;
extern volatile note NOTE;
#endif