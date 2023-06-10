#include <stdint.h>
#include <variables.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <sequence.h>
#include <playback.h>
#include <variables.h>
#include <stdio.h>
extern volatile uint8_t pb_released;
extern volatile uint8_t uart_in;
extern volatile state GAME_STATE;
extern volatile int8_t octave;
extern uint32_t STATE_LFSR;
extern volatile uint32_t u_idx;
extern volatile uint32_t idx;
extern volatile uint8_t level;
extern volatile int8_t STEP;
extern volatile uint8_t len;
extern volatile buzzer_state BUZZER;
static serial_state SERIAL_STATE;
uint32_t LFSR_PAYLOAD;
/* 
tri state variable where:
2 - created new payload and to be accepted at next fail
1 - override init state
0 - empty
*/
uint8_t payload_set = 0;
uint8_t parsed_result;
uint8_t count;
uint8_t hexchar_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    else
        return 16; // Invalid
}
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
    switch (SERIAL_STATE)
    {
        case (AWAITING_COMMAND):
        {
            switch (rx_data)
            {
                case ',':
                case 'k':
                {
                    if (octave < OCTAVES_MAX)
                    {
                        octave++;
                    }
                    break;
                }
                case '.':
                case 'l':
                {
                    if (octave > OCTAVES_MIN)
                    {
                        octave--;
                    }
                    break;
                }
                case '9':
                case 'o':
                {
                    SERIAL_STATE = AWAITING_PAYLOAD;
                }
            }
        }
        case AWAITING_PAYLOAD:
        {
            parsed_result = hexchar_to_int((char)rx_data);
            if (parsed_result != 16)
            {
                LFSR_PAYLOAD = (LFSR_PAYLOAD << 4) | parsed_result;
            }
            if (++count >= 8)
            {
                payload_set = 2;
                SERIAL_STATE = AWAITING_COMMAND;
            }
            break;
        }
    }
    if (BUZZER == PLAY)
    {
        stop_tone();
        play_tone();
    }
    // switch (SERIAL_STATE)
    // {
    // case AWAITING_COMMAND:
    //     switch (rx_data)
    //     {
    //     case '1':
    //     case 'q':
    //         uart_in = 1;
    //         break;
    //     case '2':
    //     case 'w':
    //         uart_in = 2;
    //         break;
    //     case '3':
    //     case 'e':
    //         uart_in = 3;
    //         break;
    //     case '4':
    //     case 'r':
    //         uart_in = 4;
    //         break;
    //     case ',':
    //     case 'k':
    //         if (octave < OCTAVES_MAX)
    //             octave++;
    //         break;
    //     case '.':
    //     case 'l':
    //         if (octave > OCTAVES_MIN)
    //             octave--;
    //         break;
    //     case '0':
    //     case 'p':
    //         GAME_STATE = RESET;
    //         break;
    //     case '9':
    //     case 'o':
    //         SERIAL_STATE = AWAITING_PAYLOAD;
    //         break;
    //     }
    //     break;
    // case AWAITING_PAYLOAD:
    //     switch (rx_data)
    //     {
    //     case ',':
    //     case 'k':
    //         if (octave < OCTAVES_MAX)
    //             octave++;
    //         break;
    //     case '.':
    //     case 'l':
    //         if (octave > OCTAVES_MIN)
    //             octave--;
    //         break;
    //     default:
    //         parsed_result = hexchar_to_int((char)rx_data);
    //         if (parsed_result != 16)
    //         {
    //             LFSR_PAYLOAD = (LFSR_PAYLOAD << 4) | parsed_result;
    //         }
    //         if (++count >= 8)
    //         {
    //             payload_set = 1;
    //             SERIAL_STATE = AWAITING_COMMAND;
    //         }
    //         break;
    //     }
    //     break;
    //     break;
    // }
    // if (BUZZER == PLAY)
    // {
    //     stop_tone();
    //     play_tone();
    // }
}