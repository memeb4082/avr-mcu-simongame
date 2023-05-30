#include <stdint.h>
#include <variables.h>

volatile extern uint8_t octave;
volatile extern note NOTE;
volatile extern uint32_t STATE;
volatile extern uint32_t MASK;
uint8_t uart_getc();
void uart_putc(uint8_t c);
void uart_puts(char *string);