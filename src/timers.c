#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>
#include <playback.h>
#include <spi.h>
#include <stdint.h>
volatile uint8_t pb_state = 0xFF;
volatile uint16_t elapsed_time = 0;
volatile uint16_t playback_time = 2000;
volatile uint16_t new_playback_time = 2000;
volatile uint8_t allow_updating_playback_delay = 0;
extern volatile state GAME_STATE;
ISR(TCB0_INT_vect)
{
    // if (timerticks < 0xFFFF) timerticks++;
    // if (timerticks == (delay >> 1))
    // {
    //     stop_tone();
    // }
    elapsed_time++;
    TCB0.INTFLAGS = TCB_CAPT_bm;
}
ISR(TCB1_INT_vect)
{
    // static uint8_t c0, c1;
    // c0 = 0;
    // c1 = 0;
    // uint8_t pb_edge = pb_state ^ PORTA.IN;
    // c1 = (c1 ^ c0) & pb_edge;
    // c0 = ~c0 & pb_edge;
    // pb_state ^= (c0 & c1);
    // TCB1.INTFLAGS = TCB_CAPT_bm;
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;

    uint8_t pb_edge = pb_state ^ PORTA.IN;

    // Vertical counter
    count1 = (count1 ^ count0) & pb_edge;
    count0 = ~count0 & pb_edge;

    // New debounced state
    // Update if PB high for three samples
    pb_state ^= (count1 & count0);

    TCB1.INTFLAGS = TCB_CAPT_bm;
}