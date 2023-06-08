#ifndef UART_H
#define UART_H 1

#include <stdint.h>
#include <variables.h>
#include <stdio.h>
extern volatile int8_t octave;

uint8_t uart_getc(void);
void uart_putc(uint8_t c);
void uart_puts(char *string);
int stdio_putchar(char c, FILE *stream);
int stdio_getchar(FILE *stream);
void stdio_init(void);
uint8_t hexchar_to_int(char c);
#endif