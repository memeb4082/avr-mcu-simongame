#include <avr/io.h>
#include <stdint.h>

extern volatile uint8_t BRIGHTNESS;
void adc_init();
void brightness();