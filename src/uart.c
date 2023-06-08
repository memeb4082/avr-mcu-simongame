#include <stdint.h>
#include <variables.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <sequence.h>
#include <stdio.h>
extern volatile uint8_t uart_in;
extern volatile state GAME_STATE;
extern volatile int8_t octave;
extern volatile uint32_t STATE_LFSR;
extern volatile uint32_t LFSR_MATCH;
extern volatile uint32_t u_idx;
extern volatile uint32_t idx;
extern volatile uint8_t level;
extern volatile int8_t STEP;
extern volatile char name[4];
extern volatile uint8_t len;
extern volatile buzzer_state BUZZER;
// ------------------------  SERIAL PARSER  ------------------------
uint8_t uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ; // Wait for data
    return USART0.RXDATAL;
}
void uart_putc(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ; // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

void uart_puts(char *string)
{
    int i = 0;
    while (string[i] != '\0')
    {
        uart_putc(string[i]);
        i++;
    }
}

static int stdio_putchar(char c, FILE *stream)
{
    uart_putc(c);
    return c; // the putchar function must return the character written to the stream
}
static int stdio_getchar(FILE *stream)
{
    return uart_getc();
}
static FILE stdio = FDEV_SETUP_STREAM(stdio_putchar, stdio_getchar, _FDEV_SETUP_RW);
void stdio_init(void)
{
    // Assumes serial interface is initialised elsewhere
    stdout = &stdio;
    stdin = &stdio;
}
ISR(USART0_RXC_vect)
{
    char rx_data = USART0.RXDATAL;
    USART0.TXDATAL = rx_data;
    switch (GAME_STATE)
    {
    case USER_INPUT:
        switch (rx_data)
        {
        case '1':
        case 'q':
            uart_in = 1;
            break;
        case '2':
        case 'w':
            uart_in = 2;
            break;
        case '3':
        case 'e':
            uart_in = 3;
            break;
        case '4':
        case 'r':
            uart_in = 4;
            break;
        case ',':
        case 'k':
            if (octave < OCTAVES_MAX) octave++;
            printf("%d", TCA0.SINGLE.PERBUF);
            break;
        case '.':
        case 'l':
            if (octave > OCTAVES_MIN) octave--;
            printf("%d", TCA0.SINGLE.PERBUF);
            break;
        case '0':
        case 'p':
            STATE_LFSR = LFSR_INIT;
            LFSR_MATCH = LFSR_INIT;
            u_idx = 0;
            idx = 0;
            break;
        case '9':
        case 'o':
            STEP = next_step(&STATE_LFSR);
            break;
        }
        break;
    case NAME_INPUT:
        if (rx_data != '\n')
        {
            name[len] = rx_data;
            len++;
        }
        else
        {
            name[len] = '\0';
            GAME_STATE = UART_SCORE;
        }
    }
    if (BUZZER == PLAY)
    {
        stop_tone();
        play_tone();
    }
}