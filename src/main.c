#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <sequence.h>
#include <init.h>
#include <playback.h>
#include <variables.h>
#include <timers.h>
#include <spi.h>
#include <uart.h>
// number of rounds initially accounted for, will change later
#define R_COUNT 10
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
volatile int16_t level;
volatile uint16_t delay;
// Static size array, maybe switch to pointer or malloc fuckery later
note SEQUENCE[16];
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
            level = 0;
            INPUT = INIT;
            break;
        case SEED:
            STATE_LFSR = next_step(&MASK);
            INPUT = INIT;
            break;
        }
        // idx++;
        /*------------------------------BUTTONS------------------------------*/
        if (pb_falling & PB1)
        {
            NOTE = EHIGH;
        }
        else if (pb_falling & PB2)
        {
            NOTE = A;
        }
        else if (pb_falling & PB3)
        {
            NOTE = CSHARP;
        }
        else if (pb_falling & PB4)
        {
            NOTE = ELOW;
        }
        /*----------------------------GAME STATE-----------------------------*/
        switch (GAME_STATE)
        {
        case START:
            for (int i = 0; i < level; i++)
            {
                int8_t STEP = next_step(&MASK);
                switch (STEP & 0x03) // to only get 2 lsbs, just in case 
                {
                case 0b00:
                    //
                    SEQUENCE[i] = EHIGH;
                    break;
                case 0b01:
                    //
                    SEQUENCE[i] = CSHARP;
                    break;
                case 0b10:
                    //
                    SEQUENCE[i] = A;

                    break;
                case 0b11:
                    //
                    SEQUENCE[i] = ELOW;
                    break;
                default:
                    break;
                }
            }
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
        play_tone();
    }
}