#include <sequence.h>
uint8_t next_step(uint32_t *state)
{
    uint32_t STATE_LFSR = *state;
    uint32_t BIT = STATE_LFSR & 0x01;
    STATE_LFSR >>= 1;
    if (BIT == 1)
    {
        STATE_LFSR ^= MASK;
    }
    uint8_t STEP = STATE_LFSR & 0b11;
    return STEP;
}
