#include <sequence.h>
#include <stdlib.h>
#include <stdio.h>
#include <uart.h>
#include <variables.h>
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
void update_table(char **names, char *scores, char *name, uint16_t score)
{
    for (int i = 0; i < SCORE_TABLE_SIZE; i++)
    {
        if ((names[i] == NULL || scores[i] < score))
        {
            names[i] = name;
            scores[i] = score;
            return;
        }
        else if (scores[i] < score)
        {
            names[i] = name;
            scores[i] = score;
            return;
        }
    }
}
void show_table(char **names, char *scores)
{
    printf("\n");
    for (int i = 0; i < SCORE_TABLE_SIZE; i++)
    {
        if (names[i] != NULL)
        {
            printf("%s %d\n", names[i], scores[i]);
        }
    }
}
void clear_str(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        str[i] = '\0';
        i++;
    }
}