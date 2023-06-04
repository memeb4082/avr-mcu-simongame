#ifndef VARIABLES_H
#define VARIABLES_H 1
#include <stdint.h>
#define PB1 PIN4_bm
#define PB2 PIN5_bm
#define PB3 PIN6_bm
#define PB4 PIN7_bm
#define EHIGH 338
#define CSHARP 284
#define A 451
#define ELOW 169
#define OCTAVES 4
#define BR 9600
#define MASK 0xE2023CAB
#define LFSR_INIT 0x11230851
typedef enum
{
    START,
    PLAY_TONE,
    SHOW_SEG,
    USER_INPUT,
    SUCCESS,
    FAIL,
    FINISH,
    // TODO: Figure out all the states and shii
} state;

typedef enum
{
    PLAY,
    PAUSE
} buzzer_state;

struct user
{
    uint8_t score;
    char *name;
};
#endif
