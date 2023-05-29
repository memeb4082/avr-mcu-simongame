#include <spi.h>
void spi_write(uint8_t c)
{
    SPI0.DATA = c;
}
ISR(SPI0_INT_vect)
{
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;
}