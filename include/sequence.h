#ifndef SEQUENCE_H
#define SEQUENCE_H 1
#include <avr/io.h>
#include <stdint.h>
#include <variables.h>
uint8_t next_step(uint32_t *state);
void seed(uint32_t *state, char* s);
#endif