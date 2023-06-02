#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>

extern volatile int8_t octave;
extern volatile note NOTE;
/*
Handles buzzer tone playback of different tones at different octaves
*/
void play_tone()
{
    // switch case handles which note to playback
    switch (NOTE)
    {
    case EHIGH:
        TCA0.SINGLE.PERBUF = (int)(F_CPU / E_HIGH) >> (octave);
        break;
    case CSHARP:
        TCA0.SINGLE.PERBUF = (int)(F_CPU / C_SHARP) >> (octave);
        break;
    case A:
        TCA0.SINGLE.PERBUF = (int)(F_CPU / A_MIDDLE) >> (octave);
        break;
    case ELOW:
        TCA0.SINGLE.PERBUF = (int)(F_CPU / E_LOW) >> (octave);
        break;
    }
    // TODO: up the duty cycle, set to lower value so i dont destroy my fucking ears
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 9;
}
/*
Clears CMP0BUF on TCA0 to stop buzzer playback
*/
void stop_tone()
{
    TCA0.SINGLE.CMP0BUF = 0;
}