#include <avr/io.h>
#include "spi.h"

#include "../../config.h"
#include "gcm_old_lcd_driver.h"


#define DD_MOSI	PINB3
#define DD_MISO	PINB4
#define DD_SCK	PINB5
#define DDR_SPI	DDRB
#define DD_SS	PINB2

/*
atmega8 datasheet, page 123
SPCR = SPIE, SPE, DORD, MSTR, CPOL, CPHA, SPR1, SPR0 (D7...D0)
SPSR = SPIF, WCOL, -, -, -, -, -, SPI2x (D7...D0)

SPI2X SPR1 SPR0 SCK_freq
0 0 0 fosc/4
0 0 1 fosc/16
0 1 0 fosc/64
0 1 1 fosc/128
1 0 0 fosc/2
1 0 1 fosc/8
1 1 0 fosc/32
1 1 1 fosc/64
*/

void SPI_MasterInit(void)
{
	
	SPCR = (1<<SPE)|(1<<MSTR);			// enable SPI, SPI master
	SPSR = (1<<SPI2X);				// clock rate fsck/2
}

uint8_t SPI_MasterTransmit(uint8_t cData)
{


	SPDR = cData;

	
	while(!(SPSR & (1<<SPIF)))
		;
	return SPDR;	// Za GLCD


	return 0;
	/*
	LCD_ShiftOut(LCD_MOSI,LCD_CLK,cData);
	return 0;
	*/
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	/* Return data register */
	return SPDR;
}
