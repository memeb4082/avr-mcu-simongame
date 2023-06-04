#ifndef PLAYBACK_H
#define PLAYBACK_H 1

#include <stdint.h>
#include <variables.h>

extern volatile uint16_t NOTE;
extern volatile int8_t octave;
extern volatile buzzer_state BUZZER;
void play_tone();
void stop_tone();

#endif