#ifndef SEQUENCE_H
#define SEQUENCE_H 1
#include <avr/io.h>
#include <stdint.h>

extern uint32_t MASK;

uint8_t next_step(uint32_t *state);
#endif