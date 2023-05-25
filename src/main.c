#include <avr/io.h>
#include <stdio.h>

#include <sequence.h>

uint32_t STATE_LFSR = 0x11230851;
uint8_t BIT;
uint32_t const MASK = 0xE2023CAB;

void main()
{}