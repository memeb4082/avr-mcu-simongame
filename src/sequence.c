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
void seed(uint32_t *state, char *s)
{
    uint32_t STATE_LFSR = *state;
    for (int i = 0; i < 8; i++)
    {
        STATE_LFSR = (STATE_LFSR & ~(0xFF << (i * 4))) | (s[i] << i);
    }
}
// uint16_t get_lowest_score(scores **TABLE)
// {
//     uint16_t lowest = TABLE[0]->score;
//     for (int i = 1; i < SCORE_TABLE_SIZE; i++)
//     {
//         if (TABLE[i]->score < lowest)
//         {
//             lowest = TABLE[i]->score;
//         }
//     }
//     return lowest;
// }
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
