#ifndef SEQUENCE_H
#define SEQUENCE_H 1
#include <avr/io.h>
#include <stdint.h>
#include <variables.h>
uint8_t next_step(uint32_t *state);
void sqc_append(int8_t** SEQUENCE, uint32_t* size, int8_t val);
uint8_t assert_equal(int8_t* s1, int8_t* s2, uint32_t size);
void clear(int8_t*** ptr);
#endif