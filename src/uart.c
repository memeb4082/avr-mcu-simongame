#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <variables.h>
#include <sequence.h>
volatile extern uint8_t octave;
volatile extern note NOTE;
extern uint32_t MASK;
volatile extern uint32_t STATE;
/*
Reads character from USART (Universal Synchronous/Asynchronous Receiver/Transmitter)
@return uint8_t character from USART interface
*/
uint8_t uart_getc()
{
    while(!(USART0.STATUS & USART_RXCIF_bm));
    return USART0.RXDATAL;
}
/*
Transmits character to the microcontroller via USART (Universal Synchronous/Asynchronous Receiver/Transmitter)
@param c input character
*/
void uart_putc(uint8_t c)
{
    while(!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;
}
/*
Transmits series of characters by iterating instances of uart_putc
@param *string pointer to char array
*/
void uart_puts(char *string)
{
    int i = 0;
    while( string[i] != '\0')
    {
        uart_putc(string[i]);
        i++;
    }
}

ISR(USART0_RXC_vect)
{
    char c = USART0.RXDATAL;
    switch (c)
    {
    case '1':
    case 'q':
        NOTE = E_HIGH;
        break;
    case '2':
    case 'w':
        NOTE = C_SHARP;
        break;
    case '3':
    case 'e':
        NOTE = A_MIDDLE;
        break;
    case '4':
    case 'r':
        NOTE = E_LOW;
        break;
    case ',':
    case 'k':
        if (octave < OCTAVES)
            octave++;
        break;
    case '.':
    case 'l':
        if (octave > -OCTAVES)
            octave--;
        break;
    case '0':
    case 'p':
        // TODO: Figure out what this means
        break;
    case '9':
    case 'o':
        STATE = next_step(&MASK);
        break;
    }
}