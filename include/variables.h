#ifndef VARIABLES_H
#define VARIABLES_H 1
#include <stdint.h>
#define PB1 PIN4_bm
#define PB2 PIN5_bm
#define PB3 PIN6_bm
#define PB4 PIN7_bm
#define E_HIGH 338
#define C_SHARP 284
#define A_MIDDLE 451
#define E_LOW 169
#define OCTAVES 3
#define BR 9600

// TODO: check if seed vars are right ig, cause autograder fucked up (worked, bad)
uint32_t STATE_LFSR = 0x11230851;
uint32_t MASK = 0xE2023CAB;
volatile int8_t note;
volatile uint8_t pb_released = 0;
volatile int8_t octave = 0;
volatile state GAME_STATE = INIT;
volatile input_state INPUT = INIT;
volatile uint32_t STATE;
volatile uint8_t score;
volatile int16_t idx;
uint8_t delay;
typedef enum
{
    INIT,
    SUCCESS,
    FAIL,
    // TODO: Figure out all the states and shii
} state;

typedef enum
{
    INIT,
    S1,
    S2,
    S3,
    S4,
    INC_FREQ,
    DEC_FREQ,
    RESET,
    SEED
} input_state;

struct user
{
    uint8_t score;
    char *name;
};

#endif