#include <avr/io.h>
#include <stdint.h>
#include <sequence.h>
#include <init.h>
#include <playback.h>
#include <variables.h>
#include <sequence.h>
#include <timers.h>
#include <spi.h>
uint32_t STATE_LFSR = 0xE2023CAB;
uint32_t MASK = 0x11230851;
volatile uint8_t pb_released = 0;
volatile state GAME_STATE = INIT;
volatile note NOTE = E_HIGH;
volatile uint32_t STATE;
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

        pb_prev = pb_new;    // Register previous PB state
        pb_state = PORTA.IN; // Sample current PB state
        // If multiple pushbuttons are on a single port, transitions for all
        // pushbuttons can be calculated in parallel using bitwise operations
        pb_falling = (pb_state ^ pb_prev) & ~pb_state;
        pb_rising = (pb_state ^ pb_prev) & pb_state;

        if (pb_falling & PB1)
        {
            NOTE = E_HIGH;
        }
        else if (pb_falling & PB2)
        {
            NOTE = A_MIDDLE;
        }
        else if (pb_falling & PB3)
        {
            NOTE = C_SHARP;
        }
        else if (pb_falling & PB4)
        {
            NOTE = E_LOW;
        }
        play_tone();
    }
}