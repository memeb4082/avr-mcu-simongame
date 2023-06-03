#include <sequence.h>
#include <stdlib.h>
/*
Pseudorandom number generator to generate seed for gameplay
@param *state pointer to state variable
@return pseudorandom seed for gameplay
*/
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
    *state = STATE_LFSR;
    return STEP;
}
void sqc_append(int8_t **SEQUENCE, uint32_t *size, int8_t val)
{
    int8_t *NEW_SEQUENCE = (int8_t *)realloc(*SEQUENCE, (*size + 1) * sizeof(int8_t));
    // TODO: add check to see if realloc works
    *SEQUENCE = NEW_SEQUENCE;
    (*SEQUENCE)[*size] = val;
    (*size)++;
}
// maybe change to void pointers for more flexibility idk
uint8_t assert_equal(int8_t *s1, int8_t *s2, uint32_t size)
{
    for (int i = 0; i < size; i++)
    {
        if (s1[i] != s2[i])
        {
            return 0;
        }
    }
    return 1;
}
void clear(int8_t ***ptr)
{
    if (*ptr != NULL)
    {
        free(*ptr);
        *ptr = NULL;
    }
}