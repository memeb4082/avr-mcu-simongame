#ifndef SEQUENCE_H
#define SEQUENCE_H 1
#include <avr/io.h>
#include <stdint.h>
#include <variables.h>
uint8_t next_step(uint32_t *state);
void seed(uint32_t *state, char *s);
void update_table(char **names, char *scores, char *name, uint16_t score);
void show_table(char **names, char *scores);
void clear_str(char *str);
#endif