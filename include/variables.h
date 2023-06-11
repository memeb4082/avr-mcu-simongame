#ifndef VARIABLES_H
#define VARIABLES_H 1
#include <stdint.h>
#define PB1 PIN4_bm
#define PB2 PIN5_bm
#define PB3 PIN6_bm
#define PB4 PIN7_bm
#define EHIGH 9862
#define CSHARP 11737
#define A 7391
#define ELOW 19724
#define OCTAVES_MAX 3
#define OCTAVES_MIN -2
#define SPI_FAIL 0b1110111
#define SPI_SUCCESS 0b0000000
#define BR 9600
#define MASK 0xE2023CAB
#define LFSR_INIT 0x11230851
#define WAIT_FAIL 5000 // TODO: Change to 5000, lower for testing
#define SCORE_TABLE_SIZE 5
#define BAUD_RATE 1389

typedef enum {
    START,
    OUTPUT,
    INPUT,
    SUCCESS,
    SCORE,
    RESET,
    FAIL
} state;

typedef enum
{
    PLAY,
    PAUSE
} buzzer_state;

typedef enum
{
    AWAITING_COMMAND,
    AWAITING_PAYLOAD,
    AWAITING_NAME,
} serial_state;

// typedef struct
// {
//     uint16_t score;
//     char name[20];
// } scores;
#endif
