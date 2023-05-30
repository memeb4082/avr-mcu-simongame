#ifndef PLAYBACK_H
#define PLAYBACK_H 1

#include <stdint.h>
#include <variables.h>

extern volatile int8_t octave;
extern volatile note NOTE;

void play_tone();
void stop_tone();

#endif