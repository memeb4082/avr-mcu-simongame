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
volatile note NOTE;
volatile uint8_t pb_released = 0;
volatile int8_t octave = -OCTAVES;
volatile state GAME_STATE = START;
volatile input_state INPUT = INIT;
volatile uint32_t STATE;
volatile uint8_t score;
volatile uint32_t level = 2;
volatile uint16_t delay;
volatile int8_t STEP;
extern volatile uint16_t playback_time;
int main()
{
    init();
    // Clear DISP
    spi_write(0xFF);
    // // PB State
    // uint8_t pb_prev = 0xFF;
    // uint8_t pb_state_n = 0xFF;
    // uint8_t pb_falling, pb_rising;
    // Push button states
    uint8_t pb_previous_state = 0xFF;
    uint8_t pb_new_state = 0xFF;
    uint8_t pb_falling_edge, pb_rising_edge;

    int8_t *SEQUENCE;       // pointer fuckery
    int8_t *INPUT_SEQUENCE; // seperate thing for user input to match later
    // Stores index of sequence, i.e., size and shit ig
    uint32_t idx = 0;
    // Stores index of user input
    uint32_t u_idx = 0;

    while (1)
    {
        /*
        Normalizes ADC input to a value between 0 and 175
        Allows to set the range 0.25 seconds to 2 seconds with
        an interval of 10ms times 10 to get timerticks (timer ticks every millisecond)
        TODO: Switch to ISR maybe
        */
        delay = (((175 * ((ADC0.RESULT))) >> 16) + 25) * 10;
        // // pb_new = pb_state; // Sample current PB state
        // // // If multiple pushbuttons are on a single port, transitions for all
        // // // pushbuttons can be calculated in parallel using bitwise operations
        // // // Prev debounced state
        // // // Compare edge and new/prev states
        // // pb_falling = (pb_prev ^ pb_new) & ~pb_prev;
        // // pb_rising = (pb_state ^ pb_new) & pb_new;
        // pb_prev = pb_state_n;    // Register previous PB state
        // pb_state_n = pb_state; // Sample current PB state
        // // If multiple pushbuttons are on a single port, transitions for all
        // // pushbuttons can be calculated in parallel using bitwise operations
        // pb_falling = (pb_prev ^ pb_state_n) & ~pb_prev;
        // pb_rising = (pb_prev ^ pb_state_n) & pb_state_n;
        // Previous debounced state
        pb_previous_state = pb_new_state;
        pb_new_state = pb_state;

        // Compare edge & prev/new state
        pb_falling_edge = (pb_previous_state ^ pb_new_state) & pb_previous_state;
        pb_rising_edge = (pb_previous_state ^ pb_new_state) & pb_new_state;
        /*----------------------------INPUTSTATE-----------------------------*/
        switch (INPUT)
        {
        case INIT:
            break;
        case INC_FREQ:
            if (octave < OCTAVES)
                octave++;
            INPUT = INIT;
            break;
        case DEC_FREQ:
            if (octave > -OCTAVES)
                octave--;
            INPUT = INIT;
            break;
        case RESET:
            octave = 0;
            level = 0;
            INPUT = INIT;
            // TODO: add more stuff to reset
            break;
        case SEED:
            STATE_LFSR = next_step(&STATE_LFSR);
            INPUT = INIT;
            break;
        default:
            break;
        }
        if (pb_falling_edge & PB1)
        {
            INPUT = S1;
        }
        else if (pb_falling_edge & PB2)
        {
            INPUT = S2;
        }
        else if (pb_falling_edge & PB3)
        {
            INPUT = S3;
        }
        else if (pb_falling_edge & PB4)
        {
            INPUT = S4;
        }
        /*----------------------------GAME STATE-----------------------------*/
        switch (GAME_STATE)
        {
        case START:
            elapsed_time = 0;
            STEP = next_step(&STATE_LFSR);

            switch (STEP)
            {
            case 0b00:
                NOTE = EHIGH;
                GAME_STATE = PLAYING_TONE;
                sqc_append(&SEQUENCE, &idx, 1);
                break;
            case 0b01:
                NOTE = CSHARP;
                GAME_STATE = PLAYING_TONE;
                sqc_append(&SEQUENCE, &idx, 2);
                break;
            case 0b10:
                NOTE = A;
                GAME_STATE = PLAYING_TONE;
                sqc_append(&SEQUENCE, &idx, 3);
                break;
            case 0b11:
                NOTE = ELOW;
                GAME_STATE = PLAYING_TONE;
                sqc_append(&SEQUENCE, &idx, 4);
                break;
            default:
                break;
            }
            play_tone();
            break;
        case PLAYING_TONE:
            if (elapsed_time >= (delay >> 1))
            {
                stop_tone();
                GAME_STATE = DISPLAY_SEG;
            }
        case DISPLAY_SEG:
            if ((elapsed_time >= delay) & (idx == level))
            {
                stop_tone();
                spi_write(0xFF);
                GAME_STATE = AWAITING_SEQUENCE;
            }
            else if (elapsed_time >= delay)
            {
                GAME_STATE = START;
            }
            break;
        case AWAITING_SEQUENCE:

            switch (INPUT)
            {
            case INIT:
                break;
            case S1:
                NOTE = EHIGH;
                INPUT = INIT;
                elapsed_time = 0;
                sqc_append(&INPUT_SEQUENCE, &u_idx, 1);
                uart_putc('a');
                // STATE = Pl
                break;
            case S2:
                NOTE = CSHARP;
                INPUT = INIT;
                elapsed_time = 0;
                sqc_append(&INPUT_SEQUENCE, &u_idx, 2);
                uart_putc('b');
                // STATE = Pl
                break;
            case S3:
                NOTE = A;
                INPUT = INIT;
                elapsed_time = 0;
                sqc_append(&INPUT_SEQUENCE, &u_idx, 3);
                uart_putc('c');
                // STATE = Pl
                break;
            case S4:
                NOTE = ELOW;
                INPUT = INIT;
                elapsed_time = 0;
                sqc_append(&INPUT_SEQUENCE, &u_idx, 4);
                uart_putc('d');
                // STATE = Pl
                break;
            default:
                break;
            }
            if (u_idx == 0)
                uart_putc('2');
            if (idx == u_idx)
            {
                if (assert_equal(SEQUENCE, INPUT_SEQUENCE, idx) == 0)
                {
                    GAME_STATE = FAIL;
                }
                else
                {
                    GAME_STATE = SUCCESS;
                }
            }
            break;
        case SUCCESS:
            if (elapsed_time >= delay)
            {
                // GAME_STATE = START;
                idx = 0;
                u_idx = 0;
                level++;
            }
            break;
        case FAIL:
            idx = 0;
            u_idx = 0;
            level = 1;
            // GAME_STATE = START;
            break;
        case FINISH:
            // play_tone();
            break;
        }
        /*-------------------------------------------------------------------*/
    }
}