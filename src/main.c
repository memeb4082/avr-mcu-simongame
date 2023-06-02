#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <sequence.h>
#include <init.h>
#include <playback.h>
#include <variables.h>
#include <timers.h>
#include <spi.h>
#include <uart.h>
// TODO: check if seed vars are right ig, cause autograder fucked up (worked, bad)
uint32_t STATE_LFSR = 0x11230851;
uint32_t MASK = 0xE2023CAB;

volatile note NOTE;
volatile uint8_t pb_released = 0;
volatile int8_t octave = 0;
volatile state GAME_STATE = START;
volatile input_state INPUT = INIT;
volatile uint16_t timerticks = 0xFFFF;
volatile uint32_t STATE;
volatile uint8_t score;
volatile int16_t idx;
volatile uint16_t delay;
char c;
// Stores sequences generated ig

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
    while (1)
    {
        /*
        Normalizes ADC input to a value between 0 and 175
        Allows to set the range 0.25 seconds to 2 seconds with
        an interval of 10ms times 10 to get timerticks (timer ticks every millisecond)
        TODO: Switch to ISR maybe
        */
        delay = ((175 * ((ADC0.RESULT >> 15))) + 25) * 10;
        pb_prev = pb_new;    // Register previous PB state
        pb_state = PORTA.IN; // Sample current PB state
        // If multiple pushbuttons are on a single port, transitions for all
        // pushbuttons can be calculated in parallel using bitwise operations
        pb_falling = (pb_state ^ pb_prev) & ~pb_state;
        pb_rising = (pb_state ^ pb_prev) & pb_state;
        /*----------------------------INPUTSTATE-----------------------------*/
        switch (INPUT)
        {
        case INIT:
            break;
        case S1:
            NOTE = EHIGH;
            INPUT = INIT;
            break;
        case S2:
            NOTE = CSHARP;
            INPUT = INIT;
            break;
        case S3:
            NOTE = A;
            INPUT = INIT;
            break;
        case S4:
            NOTE = ELOW;
            INPUT = INIT;
            break;
        case INC_FREQ:
            // if (octave <  (2 * OCTAVES))
            // {
            octave++;
            // }
            INPUT = INIT;
            break;
        case DEC_FREQ:
            // if (octave != 0)
            // {
            octave--;
            // }
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
        /*-------------------------------USART-------------------------------*/
        c = uart_getc();
        // TODO: Check if correct null character
        if (c != '\0')
        {
            switch (c)
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
            }
        }
        /*------------------------------BUTTONS------------------------------*/
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
        /*----------------------------GAME STATE-----------------------------*/
        switch (GAME_STATE)
        {
        case START:

            break;
        case AWAITING_SEQUENCE:
            break;
        case SUCCESS:
            break;
        case FAIL:
            break;
        case FINISH:
            break;
        }
        /*-------------------------------------------------------------------*/
    }
}
// ISR(USART0_RXC_vect)
// {
//     uint8_t c = USART0.RXDATAL;
//     stop_tone();
// }