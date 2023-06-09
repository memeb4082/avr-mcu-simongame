#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>

extern volatile int8_t octave;
extern volatile uint16_t NOTE;
extern volatile buzzer_state BUZZER;
/*
Handles buzzer tone playback of different tones at different octaves
*/
void play_tone()
{
    BUZZER = PLAY;
    if (octave < 0)
    {
        TCA0.SINGLE.PERBUF = ((int)(F_CPU / NOTE)) << (-octave);
    } else
    {
        TCA0.SINGLE.PERBUF = ((int)(F_CPU / NOTE)) >> (octave);
    }
    // TODO: up the duty cycle, set to lower value so i dont destroy my fucking ears
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
}
/*
Clears CMP0BUF on TCA0 to stop buzzer playback
*/
void stop_tone()
{
    BUZZER = PAUSE;
    TCA0.SINGLE.CMP0BUF = 0;
}