#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>
#include <playback.h>
#include <spi.h>
#include <stdint.h>
volatile uint8_t pb_state = 0xFF;
volatile uint16_t elapsed_time = 0;
extern volatile state GAME_STATE;
ISR(TCB0_INT_vect)
{
    // if (timerticks < 0xFFFF) timerticks++;
    // if (timerticks == (delay >> 1))
    // {
    //     stop_tone();
    // }
    elapsed_time++;
    if (GAME_STATE == PLAYING_TONE)
    {
        spi_write(0xFF);
    }
    else if (GAME_STATE == FAIL)
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
    else if (GAME_STATE == SUCCESS)
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
    else
    {
        switch (NOTE)
        {
        case EHIGH:
            spi_write(0b0111110 | (0x01 << 7));
            break;
        case CSHARP:
            spi_write(0b1101011 | (0x01 << 7));
            break;
        case A:
            spi_write(0b0111110);
            break;
        case ELOW:
            spi_write(0b1101011);
            break;
        }
    }
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