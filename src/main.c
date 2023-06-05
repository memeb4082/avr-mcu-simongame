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
volatile uint32_t STATE_LFSR = LFSR_INIT;
volatile uint32_t LFSR_MATCH = LFSR_INIT;
volatile uint16_t NOTE;
volatile uint8_t pb_released = 0;
volatile int8_t octave = -OCTAVES;
volatile uint32_t STATE;
volatile uint8_t score;
volatile uint32_t level = 1;
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
// Stores SPI thingo
uint8_t spi;
// Stores index of playback and shii
volatile uint32_t idx = 0;
// Stores index of user and shii
volatile uint32_t u_idx = 0;
// Stores input of user and shii
uint8_t input;
// Bool var sorta
uint8_t valid = 1;
volatile state GAME_STATE = START;
volatile buzzer_state BUZZER = PAUSE;
volatile uint8_t uart_in;
volatile char name[4];
volatile uint8_t len;
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
        delay = (0.855 * (ADC0.RESULT >> 5)) + 250;
        // delay = 1;
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
                    if ((pb_rising_edge & (PB1 | PB2 | PB3 | PB4)) | (uart_in > 0))
                    {
                        pb_released = 1;
                    }
                }
                else if (elapsed_time >= delay)
                {
                    spi = 0xFF;
                    uart_in = 0;
                    stop_tone();
                    /* Match user input with sequence */
                    if (next_step(&LFSR_MATCH) == input)
                    {
                        valid *= 1;
                    }
                    else
                    {
                        valid *= 0;
                    }
                }
                break;
            case PAUSE:
                // int cmp instead of str cmp, faster i think
                /*
                qwer -> 113 119 101 114
                1234 ->  49  50  51  52
                */
                if ((pb_falling_edge & PB1) | (uart_in == 1))
                {
                    NOTE = EHIGH;
                    play_tone();
                    pb_released = 0;
                    elapsed_time = 0;
                    spi = 0b0111110 | (0x01 << 7);
                    input = 1;
                    u_idx++;
                }
                else if ((pb_falling_edge & PB2) | (uart_in == 2))
                {
                    NOTE = CSHARP;
                    play_tone();
                    pb_released = 0;
                    elapsed_time = 0;
                    spi = 0b1101011 | (0x01 << 7);
                    input = 2;
                    u_idx++;
                }
                else if ((pb_falling_edge & PB3) | (uart_in == 3))
                {
                    NOTE = A;
                    play_tone();
                    pb_released = 0;
                    elapsed_time = 0;
                    spi = 0b0111110;
                    input = 3;
                    u_idx++;
                }
                else if ((pb_falling_edge & PB4) | (uart_in == 4))
                {
                    NOTE = ELOW;
                    play_tone();
                    pb_released = 0;
                    elapsed_time = 0;
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
                STATE_LFSR = LFSR_INIT;
                LFSR_MATCH = LFSR_INIT;
                u_idx = 0;
                idx = 0;
                level++;
                GAME_STATE = START;
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
                GAME_STATE = DISP_SCORE;
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
        case DISP_SCORE:
            level %= 100;
            ones = level % 10;
            while (level > 10)
            {
                level -= 10;
                tens++;
            }
            if ((elapsed_time % 2) == 1)
            {
                spi_write(segs[ones]);
            }
            else
            {
                spi_write(segs[tens] | (0x01 << 7));
            }
            if (elapsed_time >= delay)
            {
                uart_puts("Enter name:");
                GAME_STATE = NAME_INPUT;
            }
            // TODO: move vars to top maybe
            break;
        case NAME_INPUT:
            if ((elapsed_time % 2) == 1)
            {
                spi_write(segs[ones]);
            }
            else
            {
                spi_write(segs[tens] | (0x01 << 7));
            }
            break;
        case UART_SCORE:
            uart_puts("\n Your score ");
            uart_puts(name);
            uart_puts(" is ");
            uart_putc(level + 48); // make this work up 65535
            GAME_STATE = FINISH;
            break;
        case RANKINGS:
            break;
        case FINISH:
            break;
        }
    }
}