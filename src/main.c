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
// TODO: check if seed vars are right ig, cause autograder fucked up (worked, bad)
uint32_t STATE_LFSR = LFSR_INIT;  // LFSR state for creating random sequence
uint32_t STATE_MATCH = LFSR_INIT; // LFSR state to match against random sequence
uint16_t NOTE;
volatile uint8_t pb_released = 0;
volatile int8_t octave = 0;
uint8_t score;
uint16_t level = 1;
volatile uint16_t delay;
int8_t STEP;
extern volatile uint16_t playback_time;
extern volatile serial_state SERIAL_STATE;
extern uint32_t LFSR_PAYLOAD;
extern uint8_t payload_set;
// Thingy to store spi out for numbers
int8_t segs[10] = {
    0b0001000,
    0b1101011,
    0b1000100,
    0b1000001,
    0b0100011,
    0b0010001,
    0b0010000,
    0b1001011,
    0b0000000,
    0b0000001};
volatile state GAME_STATE = START;
volatile state PREV;
volatile buzzer_state BUZZER = PAUSE;
volatile uint8_t uart_in;
volatile uint8_t len;
const uint32_t SHIFT_MASK = 0xFFFFFFFF;
scores SCORES[5];
extern scores PLAYER;
int main()
{
    // initialise stuff
    init();
    // initialise printf
    stdio_init();
    // Clear DISP
    spi_write(0xFF);
    // Push button states
    uint8_t pb_previous_state = 0xFF;
    uint8_t pb_new_state = 0xFF;
    uint8_t pb_falling_edge, pb_rising_edge;
    uint8_t tens = 0;
    uint8_t ones = 0;
    uint8_t STEP;
    uint8_t score;
    // Stores SPI thingo
    uint8_t spi;
    // Stores index of playback and shii
    uint32_t idx = 0;
    // Stores index of user and shii
    uint32_t u_idx = 0;
    // Stores input of user and shii
    uint8_t input;
    // Bool var sorta
    while (1)
    {
        /*
        Normalizes ADC input to a value between 0 and 175
        Allows to set the range 0.25 seconds to 2 seconds with
        an interval of 10ms times 10 to get timerticks (timer ticks every millisecond)
        TODO: Switch to ISR maybe
        */
        delay = (117 * (ADC0.RESULT >> 12)) + 248;
        /*
        If multiple pushbuttons are on a single port, transitions for all
        pushbuttons can be calculated in parallel using bitwise operations
        Prev debounced state
        Compare edge and new/prev states
        If multiple pushbuttons are on a single port, transitions for all
        pushbuttons can be calculated in parallel using bitwise operations
        Previous debounced state
         */
        pb_previous_state = pb_new_state;
        pb_new_state = pb_state;
        // Compare edge & prev/new state
        pb_falling_edge = (pb_previous_state ^ pb_new_state) & pb_previous_state;
        pb_rising_edge = (pb_previous_state ^ pb_new_state) & pb_new_state;
        /*-------------------------------------------------------------------------*/
        switch (GAME_STATE)
        {
        case START:
        {
            if (idx == level) // check if the number of tones outputted is equal to level
            {
                GAME_STATE = INPUT;
            }
            else
            {
                elapsed_time = 0; // reset elapsed time
                STEP = next_step(&STATE_LFSR);
                switch (STEP)
                {
                case 1:
                {
                    NOTE = EHIGH;
                    spi = 0b0111110 | (1 << 7);
                    break;
                }
                case 2:
                {
                    NOTE = CSHARP;
                    spi = 0b1101011 | (1 << 7);
                    break;
                }
                case 3:
                {
                    NOTE = A;
                    spi = 0b0111110;
                    break;
                }
                case 4:
                {
                    NOTE = ELOW;
                    spi = 0b1101011;
                    break;
                }
                }
                idx++; // increments the index of the tones played
                GAME_STATE = OUTPUT;
            }
            break;
        }
        case OUTPUT:
        {
            if (elapsed_time < (delay >> 1))
            {
                play_tone();
                spi_write(0xFF); // clear DISP
            }
            else if (elapsed_time < delay)
            {
                stop_tone();
                spi_write(spi); // write the current tone to DISP
            }
            else if (elapsed_time >= delay)
            {
                spi_write(0xFF); // clear DISP
                GAME_STATE = START;
            }
            break;
        }
        case INPUT:
        {
            spi_write(spi);
            switch (BUZZER)
            {
            case PLAY:
            {
                if (!pb_released)
                {
                    if ((pb_rising_edge & (PB1 | PB2 | PB3 | PB4)) | (uart_in > 0))
                    {
                        pb_released = 1;
                    }
                }
                else if (elapsed_time >= delay)
                {
                    stop_tone();
                    uart_in = 0;
                    elapsed_time = 0;
                    if (input == next_step(&STATE_MATCH))
                    {
                        if (u_idx == level)
                        {
                            score = level % 100;
                            ones = level % 10;
                            while (score > 9)
                            {
                                score -= 10;
                                tens++;
                            }
                            level++;
                            u_idx = 0;
                            idx = 0;
                            printf("SUCCESS\n");
                            GAME_STATE = SUCCESS;
                        }
                    }
                    else
                    {
                        // valid *= 0;
                        printf("GAME OVER\n");
                        printf("Enter name: ");
                        u_idx = 0;
                        idx = 0;
                        SERIAL_STATE = AWAITING_NAME;
                        GAME_STATE = FAIL;
                    }
                }
                break;
            }
            case PAUSE:
            {
                if ((pb_falling_edge & PB1) | (uart_in == 1))
                {
                    NOTE = EHIGH;
                    input = 1;
                    spi = 0b0111110 | (1 << 7);
                    u_idx++;
                    elapsed_time = 0; // increments the index of the tones played
                    play_tone();
                }
                else if ((pb_falling_edge & PB2) | (uart_in == 2))
                {
                    NOTE = CSHARP;
                    input = 2;
                    spi = 0b1101011 | (1 << 7);
                    u_idx++;
                    elapsed_time = 0;
                    play_tone();
                }
                else if ((pb_falling_edge & PB3) | (uart_in == 3))
                {
                    NOTE = A;
                    input = 3;
                    spi = 0b0111110;
                    u_idx++;
                    elapsed_time = 0;
                    play_tone();
                }
                else if ((pb_falling_edge & PB4) | (uart_in == 4))
                {
                    NOTE = ELOW;
                    input = 4;
                    spi = 0b1101011;
                    u_idx++;
                    elapsed_time = 0;
                    play_tone();
                }
                else
                {
                    spi = 0xFF;
                }
                break;
            }
            }
            break;
        }
        case FAIL:
        {
            if ((payload_set == 2))
            {
                STATE_LFSR = STATE_MATCH; // start the new sequence from the one after the wrong input
                payload_set = 1;
            }
            if (elapsed_time >= WAIT_FAIL)
            {
                SERIAL_STATE = AWAITING_COMMAND;
                if (!(search_table(&SCORES, &PLAYER)))
                {
                    update_table(&SCORES, &PLAYER);
                }
                show_table(&SCORES, level);
                level = 1;
                GAME_STATE = START;
            }
            else
            {
                if ((elapsed_time % 2) == 1)
                {
                    spi_write(SPI_FAIL);
                }
                else
                {
                    spi_write(SPI_FAIL | (1 << 7));
                }
            }
            
            break;
        }
        case SUCCESS:
        {
            if (elapsed_time >= delay)
            {
                if (payload_set > 0)
                {
                    STATE_LFSR = LFSR_PAYLOAD;
                    STATE_MATCH = LFSR_PAYLOAD;
                    payload_set = 1; // set to 1 for next fail
                }
                else
                {
                    STATE_LFSR = LFSR_INIT;
                    STATE_MATCH = LFSR_INIT;
                }
                GAME_STATE = START;
            }
            else
            {
                if ((elapsed_time % 2) == 1)
                {
                    spi_write(segs[ones]);
                }
                else
                {
                    if ((level > 9) | (tens > 0))
                    {
                        spi_write(segs[tens] | (1 << 7));
                    }
                }
            }
            break;
        }
        }
    }
}