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
uint32_t STATE_LFSR = LFSR_INIT;
uint32_t LFSR_MATCH = LFSR_INIT;
volatile uint16_t NOTE;
volatile uint8_t pb_released = 0;
volatile int8_t octave = -OCTAVES;
// volatile input_state INPUT = INIT;
volatile uint32_t STATE;
volatile uint8_t score;
volatile uint32_t level = 3;
volatile uint16_t delay;
volatile int8_t STEP;
extern volatile uint16_t playback_time;
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
int8_t *SEQUENCE;       // pointer fuckery
int8_t *INPUT_SEQUENCE; // seperate thing for user input to match later
// Stores SPI thingo
uint8_t spi;
// Stores index of playback and shii
uint32_t idx = 0;
// Stores index of user and shii
uint32_t u_idx = 0;
// Stores input of user and shii
uint8_t input;
// Bool var sorta
uint8_t valid = 1;
volatile state GAME_STATE = START;
volatile buzzer_state BUZZER = PAUSE;
volatile input_state INPUT;
int main()
{
    init();
    // Clear DISP
    spi_write(0xFF);
    // Push button states
    uint8_t pb_previous_state = 0xFF;
    uint8_t pb_new_state = 0xFF;
    uint8_t pb_falling_edge, pb_rising_edge;
    uint8_t tens = 0;
    uint8_t ones = 0;
    spi_write(0xFF);
    while (1)
    {
        /*
        Normalizes ADC input to a value between 0 and 175
        Allows to set the range 0.25 seconds to 2 seconds with
        an interval of 10ms times 10 to get timerticks (timer ticks every millisecond)
        TODO: Switch to ISR maybe
        */
        delay = (((175 * ((ADC0.RESULT))) >> 16) + 25) * 10;
        // delay = 200;
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
            if (idx == level)
            {
                stop_tone(); // stop residual tones
                spi = 0xFF;
                GAME_STATE = USER_INPUT;
            }
            else
            {
                elapsed_time = 0; // set the time to 0
                GAME_STATE = PLAY_TONE;
                STEP = next_step(&STATE_LFSR); // new step variable
                switch (STEP)
                {
                case 1:
                    NOTE = EHIGH;
                    spi = 0b0111110 | (0x01 << 7);
                    break;
                case 2:
                    NOTE = CSHARP;
                    spi = 0b1101011 | (0x01 << 7);
                    break;
                case 3:
                    NOTE = A;
                    spi = 0b0111110;
                    break;
                case 4:
                    NOTE = ELOW;
                    spi = 0b1101011;
                    break;
                }
                idx++;
            }
            break;
        case PLAY_TONE:
            // stop at half delay
            spi_write(0xFF); // clear DISP
            if (elapsed_time >= (delay >> 1))
            {
                stop_tone();
                GAME_STATE = SHOW_SEG;
            }
            else if (BUZZER == PAUSE)
            {
                play_tone();
            }
            break;
        case SHOW_SEG:
            if (elapsed_time >= delay)
            {
                GAME_STATE = START;
            }
            spi_write(spi);
            break;
        case USER_INPUT:
            // PB Stuff
            spi_write(spi);
            switch (BUZZER)
            {
            case PLAY:
                if (!pb_released)
                {
                    if (pb_rising_edge & (PB1 | PB2 | PB3 | PB4))
                    {
                        pb_released = 1;
                    }
                }
                else if (elapsed_time >= delay)
                {
                    allow_updating_playback_delay = 1;
                    spi = 0xFF;
                    stop_tone();
                    /* Match user input with sequence */
                    if (next_step(&LFSR_MATCH) == input)
                    {
                        valid *= 1;
                        uart_putc('p');
                    }
                    else
                    {
                        valid *= 0;
                        uart_putc('f');
                    }
                }
                break;
            case PAUSE:
                if (pb_falling_edge & PB1)
                {
                    NOTE = EHIGH;
                    play_tone();
                    pb_released, elapsed_time = 0;
                    spi = 0b0111110 | (0x01 << 7);
                    input = 1;
                    u_idx++;
                }
                else if (pb_falling_edge & PB2)
                {
                    NOTE = CSHARP;
                    play_tone();
                    pb_released, elapsed_time = 0;
                    spi = 0b1101011 | (0x01 << 7);
                    input = 2;
                    u_idx++;
                }
                else if (pb_falling_edge & PB3)
                {
                    NOTE = A;
                    play_tone();
                    pb_released, elapsed_time = 0;
                    spi = 0b0111110;
                    input = 3;
                    u_idx++;
                }
                else if (pb_falling_edge & PB4)
                {
                    NOTE = ELOW;
                    play_tone();
                    pb_released, elapsed_time = 0;
                    spi = 0b1101011;
                    input = 4;
                    u_idx++;
                }
                break;
            }
            if (idx == u_idx && (pb_released & (elapsed_time >= delay)))
            {
                elapsed_time = 0;
                stop_tone();
                GAME_STATE = FAIL;
                if (valid == 1)
                {
                    GAME_STATE = SUCCESS;
                }
            }
            break;
        case SUCCESS:
            if (elapsed_time >= delay)
            {
                elapsed_time = 0;
                GAME_STATE = FINISH;
            }
            else
            {
                if ((elapsed_time % 2) == 1)
                {
                    spi_write(0b0000000);
                }
                else
                {
                    spi_write(0b0000000 | (0x01 << 7));
                }
            }
            break;

        case FAIL:
            if (elapsed_time >= delay)
            {
                elapsed_time = 0;
                GAME_STATE = FINISH;
            }
            else
            {
                if ((elapsed_time % 2) == 1)
                {
                    spi_write(0b0000001);
                }
                else
                {
                    spi_write(0b0000001 | (0x01 << 7));
                }
            }
            break;
        case FINISH:
            if (ones == 0)
            {
                level %= 100;
                ones = level % 10;
                while (level > 10)
                {
                    level -= 10;
                    tens++;
                }
            }
            else
            {
                if ((elapsed_time % 2) == 1)
                {
                    spi_write(segs[ones]);
                }
                else
                {
                    spi_write(segs[tens] | (0x01 << 7));
                }
            }
            break;
        }
    }
}