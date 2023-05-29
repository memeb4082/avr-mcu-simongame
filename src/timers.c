#include <timers.h>
void init_tca()
{
    cli();
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc | TCA_SINGLE_ENABLE_bm;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0.SINGLE.PER = freq;
    TCA0.SINGLE.CMP0 = freq >> 1;
    sei();
}
void init_tcb()
{
    cli();
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc;
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;
    TCB0.INTCTRL = TCB_CAPT_bm;
    TCB0.CCMP = 3333;
    TCB0.CTRLA |= TCB_ENABLE_bm;
    sei();
}
ISR(TCA0_INT_VECT)
{

}
ISR(TCB0_INT_VECT)
{

}