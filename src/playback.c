#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <states.h>
#define E_HIGH 9866
#define C_SHARP 11732
#define A_MIDDLE 7391
#define E_LOW 19732

volatile int8_t octave = 0;
extern volatile note NOTE;
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
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
}

void stop_tone()
{
    TCA0.SINGLE.CMP0BUF = 0;
}

ISR(USART0_RXC_vect)
{
    char c = USART0.RXDATAL;
    if (c == ',')
        c = 'k';
    if (c == '.')
        c = 'l';
    switch (c)
    {
    case 'k':
        if (octave < 4)
            octave++;
        break;
    case 'l':
        if (octave > -4)
            octave--;
        break;
    }
}