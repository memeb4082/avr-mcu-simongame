#include <stdint.h>
#include <variables.h>
#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile input_state INPUT;
extern volatile state GAME_STATE;
extern volatile int8_t octave;
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
uint8_t hexchar_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    else
        return 16; // Invalid
}

ISR(USART0_RXC_vect)
{
    char rx_data = USART0.RXDATAL;
    USART0.TXDATAL = rx_data;
    switch (rx_data)
    {
    case '1':
    case 'q':
        INPUT = S1;
        break;
    case '2':
    case 'w':
        INPUT = S2;
        break;
    case '3':
    case 'e':
        INPUT = S3;
        break;
    case '4':
    case 'r':
        INPUT = S4;
        break;
    case ',':
    case 'k':
        octave++;
        break;
    case '.':
    case 'l':
        octave--;
        break;
    case '0':
    case 'p':
        INPUT = RESET;
        break;
    case '9':
    case 'o':
        INPUT = SEED;
        break;
    }
}