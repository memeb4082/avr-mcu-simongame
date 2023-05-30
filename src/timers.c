#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>
#include <spi.h>
volatile uint8_t pb_state = 0xFF;
volatile uint16_t playback = 1;
volatile uint16_t elapsed = 0;
volatile uint16_t new_time = 1;
volatile uint8_t allow_update = 0;
extern volatile uint8_t DISP;
extern volatile note NOTE;
ISR(TCB0_INT_vect)
{
    elapsed++;

    if (allow_update)
    {
        playback = new_time;
        allow_update = 0;
    }
    switch (NOTE)
    {
    case EHIGH:
        spi_write(0b0111110);
        break;
    case CSHARP:
        spi_write(0b1101011);
        break;
    case A:
        spi_write(0b0111110 | (0x01 << 7));
        break;
    case ELOW:
        spi_write(0b1101011 | (0x01 << 7));
        break;
    }
    TCB0.INTFLAGS = TCB_CAPT_bm;
}
ISR(TCB1_INT_vect)
{
    static uint8_t c0, c1;
    c0 = 0;
    c1 = 0;
    uint8_t pb_edge = pb_state ^ PORTA.IN;
    c1 = (c1 ^ c0) & pb_edge;
    c0 = ~c0 & pb_edge;
    pb_state ^= (c0 & c1);
    TCB1.INTFLAGS = TCB_CAPT_bm;
}