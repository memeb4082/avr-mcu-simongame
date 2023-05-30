#ifndef VARIABLES_H
#define VARIABLES_H 1

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

typedef enum
{
    INIT,
    SUCCESS,
    FAIL,
    // TODO: Figure out all the states and shii
} state;

typedef enum
{
    EHIGH,
    CSHARP,
    A,
    ELOW
} note;
#endif
