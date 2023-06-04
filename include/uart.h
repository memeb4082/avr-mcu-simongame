#ifndef UART_H
#define UART_H 1

#include <stdint.h>
#include <variables.h>

extern volatile int8_t octave;

uint8_t uart_getc(void);
void uart_putc(uint8_t c);
void uart_puts(char *string);

#endif