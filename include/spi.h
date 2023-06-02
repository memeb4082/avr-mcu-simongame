#ifndef SPI_H
#define SPI_H 1

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void spi_init();
void spi_write(uint8_t b);

#endif