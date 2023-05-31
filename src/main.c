#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <sequence.h>
#include <init.h>
#include <playback.h>
#include <variables.h>
#include <sequence.h>
#include <timers.h>
#include <spi.h>
#include <uart.h>
int main()
{
    init();
    // Initial game state
    STATE = next_step(&MASK);
    // Clear DISP
    spi_write(0xFF);
    // PB State
    uint8_t pb_prev = 0xFF;
    uint8_t pb_new = 0xFF;
    uint8_t pb_falling, pb_rising;
    ADC0.RESULT >> 8;
    while (1)
    {
        /*
        Normalizes ADC input to a value between 0 and 175
        Allows to set the range 0.25 seconds to 2 seconds with
        an interval of 10ms
        TODO: Switch to ISR maybe
        */
        delay = (175 * ((ADC0.RESULT >> 15))) + 25;
        pb_prev = pb_new;    // Register previous PB state
        pb_state = PORTA.IN; // Sample current PB state
        // If multiple pushbuttons are on a single port, transitions for all
        // pushbuttons can be calculated in parallel using bitwise operations
        pb_falling = (pb_state ^ pb_prev) & ~pb_state;
        pb_rising = (pb_state ^ pb_prev) & pb_state;

        if (pb_falling & PB1)
        {
            INPUT = S1;
        }
        else if (pb_falling & PB2)
        {
            INPUT = S2;
        }
        else if (pb_falling & PB3)
        {
            INPUT = S3;
        }
        else if (pb_falling & PB4)
        {
            INPUT = S4;
        }
        /*------------------------------BUTTONS------------------------------*/
        switch (uart_getc())
        {
        case '1':
        case 'q':
            INPUT = S1;
            break;
        case '2':
        case 'w':
            INPUT = S2;
            break;
        case '3':
        case 'e':
            INPUT = S3;
            break;
        case '4':
        case 'r':
            INPUT = S4;
            break;
        case ',':
        case 'k':
            INPUT = INC_FREQ;
            break;
        case '.':
        case 'l':
            INPUT = DEC_FREQ;
            break;
        case '0':
        case 'p':
            INPUT = RESET;
            break;
        case '9':
        case 'o':
            INPUT = SEED;
            break;
        default:
            break;
        }
        play_tone(); // figure out the rest of the fucken game and shii
        switch (GAME_STATE)
        {
            case INIT:
                break;
            case S1:
                note = E_HIGH;
                INPUT = INIT;
                break;
            case S2:
                note = C_SHARP;
                INPUT = INIT;
                break;
            case S3:
                note = A_MIDDLE;
                INPUT = INIT;
                break;
            case S4:
                note = E_LOW;
                INPUT = INIT;
                break;
            case INC_FREQ:
                if (octave < OCTAVES){
                    octave++;
                }
                INPUT = INIT;
                break;
            case DEC_FREQ:
                if (octave > -OCTAVES){
                    octave--;
                }
                INPUT = INIT;
                break;
            case RESET:
                octave = 0;
                idx = 0;
                INPUT = INIT;
                break;
            case SEED:
                STATE_LFSR = next_step(&MASK);
                INPUT = INIT;
                break;
        }
        idx++;
    }
}
// ISR(USART0_RXC_vect)
// {
//     uint8_t c = USART0.RXDATAL;
//     stop_tone();
// }