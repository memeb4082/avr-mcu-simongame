#include <avr/io.h>
#include <stdint.h>

extern volatile uint8_t freq;
void init_tca();
void init_tcb();