#include <spi.h>
/*
Writes character to SPI (Serial Peripheral Interface)
@param c character to be written to SPI
*/
void spi_write(uint8_t c)
{
    SPI0.DATA = c;
}
ISR(SPI0_INT_vect)
{
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;
    SPI0.INTFLAGS |= (1 << SPI_RXCIF_bp);
}