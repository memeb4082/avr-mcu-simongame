#ifndef STATE_H
#define STATE_H 1

#define PB1 PIN4_bm
#define PB2 PIN5_bm
#define PB3 PIN6_bm
#define PB4 PIN7_bm

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


