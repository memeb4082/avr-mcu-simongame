#ifndef INIT_H
#define INIT_H 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <variables.h>

void port_init()
{
	PORTB.DIRSET = PIN2_bm | PIN0_bm;
}

void pb_init()
{
	PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
	PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
}

void pwm_init()
{
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;
	/*
	PWM 440HZ
	TODO: change to initially frequency
	long statement to round float to int and shii
	TODO: make function for rounding or use standard library maybe
	*/
	TCA0.SINGLE.PER = ((F_CPU / 440) >= 0) ? (int)((F_CPU / 440) + 0.5) : (int)((F_CPU / 440) - 0.5);
	/*
	0% duty cycle
	TODO: add the rounding stuff ig
	*/
	TCA0.SINGLE.CMP0 = 0;
	// Enable TCA0
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
}

void timer_init()
{
	/*
	1ms tcb0 4ms tcb1
	TODO: check to replace for proper thing
	*/
	TCB0.CCMP = 3333;
	TCB0.INTCTRL = TCB_CAPT_bm;
	TCB0.CTRLA = TCB_ENABLE_bm;
	TCB0.CTRLB = TCB_CNTMODE_INT_gc;

	TCB1.CCMP = 3333 * 4;
	TCB1.INTCTRL = TCB_CAPT_bm;
	TCB1.CTRLA = TCB_ENABLE_bm;
}

void uart_init()
{
	USART0.BAUD = (((4 * F_CPU) / (BR)) >= 0) ? (int)((4 * F_CPU) / (BR) + 0.5) : (int)((4 * F_CPU) / (BR)-0.5);
	USART0.CTRLA = USART_RXCIE_bm;
	USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

void spi_init()
{
	PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;
	PORTC.DIRSET = (PIN0_bm | PIN2_bm);
	PORTA.OUTSET = PIN1_bm;
	PORTA.DIRSET = PIN1_bm;
	SPI0.CTRLA = SPI_MASTER_bm;
	SPI0.CTRLB = SPI_SSD_bm;
	SPI0.INTCTRL = SPI_IE_bm;
	SPI0.CTRLA |= SPI_ENABLE_bm;
}

void adc_init()
{
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_PRESC_DIV2_gc;
	ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc;
	ADC0.CTRLE = 64;
	ADC0.CTRLF = ADC_FREERUN_bm | ADC_LEFTADJ_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
	ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;
}

void init()
{
	cli();
    port_init();
    pb_init();
    pwm_init();
    spi_init();
    timer_init();
    uart_init();
	adc_init();
    sei();
}
#endif