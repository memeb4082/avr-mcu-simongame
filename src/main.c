#include <avr/io.h>
#include <stdint.h>
#include <sequence.h>
#include <init.h>
#include <playback.h>
#include <variables.h>
#include <sequence.h>
#include <timers.h>
#include <spi.h>

// TODO: check if var names match
uint32_t STATE_LFSR = 0x11230851;
uint32_t MASK = 0xE2023CAB;
volatile uint8_t pb_released = 0;
volatile int8_t octave = 2;
volatile state GAME_STATE = INIT;
volatile note NOTE = EHIGH;
volatile uint32_t STATE;
char c;
uint8_t uart_getc(void) {
    while (!(USART0.STATUS & USART_RXCIF_bm));  // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm));  // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

void uart_puts(char * string) {
    int i = 0;
    while(string[i] != '\0') {
        uart_putc(string[i]);
        i++;
    }
}
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
            NOTE = ELOW;
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
            NOTE = EHIGH;
        }
        play_tone();
        c = uart_getc();
        if (c == 'k')
        {
            octave--;
        }
        // ------------------- BUTTONS -------------------- //
        // uart_putc('k');
        // play_tone(uart_getc());
    }
}
