#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <sequence.h>

#define E328 10163
#define C284 11737
#define A440 7576
#define ELOW 19724

volatile uint16_t freq = E328;
volatile uint32_t STATE_LFSR = 0x11230851;
volatile uint8_t BIT;
uint32_t const MASK = 0xE2023CAB;
void init_tca(void)
{
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.PER = freq;
    TCA0.SINGLE.CMP0 = freq >> 1;
}
void init_tcb(void)
{
    cli();
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc;
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.CCMP = 3333;
    TCB0.CTRLA |= TCB_ENABLE_bm;
    sei();
}
void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm; // Enables the ADC
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;
    ADC0.CTRLC = ADC_TIMEBASE4_bm | ADC_REFSEL_VDD_gc;
    ADC0.CTRLE = 64; // Sample duration
    ADC0.CTRLF = ADC_FREERUN_bm; // Sets the ADC xto free running mode
    ADC0.CTRLF |= ADC_LEFTADJ_bm; // Left adjusts the contents of the result
    /* Sets the ADC to read from the pin connected to the potentiometer */
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    ADC0.COMMAND |= ADC_MODE_SINGLE_8BIT_gc; // Set resolution of the result
    ADC0.COMMAND |= ADC_START_IMMEDIATE_gc; // Start the ADC
}
int main(void)
{
    // Enable buzzer output
    PORTB.DIRSET = PIN0_bm;
    PORTB.OUTSET = PIN0_bm;
    // Enable buttons
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
    // Initialize timers
    init_tca();
    init_tcb();
}