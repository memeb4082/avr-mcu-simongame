#include <stdint.h>

extern volatile uint32_t STATE_LFSR;
extern volatile uint8_t BIT;
extern uint32_t const MASK;
uint8_t next_step();
