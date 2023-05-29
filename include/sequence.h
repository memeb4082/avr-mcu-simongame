#include <avr/io.h>
#include <stdint.h>

extern volatile uint32_t MASK;

uint8_t next_step(uint32_t *state);
