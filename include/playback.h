#ifndef PLAYBACK_H
#define PLAYBACK_H 1

#include <stdint.h>
#include <variables.h>

extern volatile note NOTE;
extern volatile int8_t octave;
void play_tone();
void stop_tone();

#endif