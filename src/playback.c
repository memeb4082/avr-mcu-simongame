#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>

extern volatile int8_t octave;
extern volatile int8_t note;
/*
Handles buzzer tone playback of different tones at different octaves
*/
void play_tone()
{
    TCA0.SINGLE.PERBUF = (int)(F_CPU / note) >> (octave);
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
}
/*
Clears CMP0BUF on TCA0 to stop buzzer playback
*/
void stop_tone()
{
    TCA0.SINGLE.CMP0BUF = 0;
}