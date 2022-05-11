/*
 * DISP_3000_driver.c
 *
 * Created: 10.05.2022 19:16:15
 *  Author: qfj
 */ 

#include "DISP_3000_driver.h"


#define RELEASE_DISPLAY 1.03

// HISTORY -------------------------------------------------------------
// 1.00 - First release on March 2016
// 1.03 - See comments at the end of this file
// 1.03_custom - adapted HZB Klaus Kiefer 2018
// 1.03_withoffset - adapted HZB Klaus Kiefer 2019

// (c) 2007 Speed-IT-up (Display3000), Peter und Stefan Küsters
// es ist gestattet, diese Routinen in eigene Programme einzubauen,
// wenn die folgenden 6 eingrahmten Zeilen im Kopf Ihres Sourcecodes stehen.
//
// ------------------------------------------------------------------------------------------------------------
// Display-Software-Grundlagen wurden von Peter Küsters, www.display3000.com ermittelt
// Dieser Display-Code ist urheberrechtlich geschützt. Sie erhalten eine Source-Code-Lizenz,
// d.h. Sie dürfen den Code in eigenen Programmen verwenden, diese aber nur in kompilierter
// Form weitergeben. Die Weitergabe dieses Codes in lesbarer Form oder die Publizierung
// im Internet etc. ist nicht gestattet und stellen einen Verstoß gegen das Urheberrecht dar.
// Weitere Displays, Platinen, Spezialstecker und fertige Module: www.display3000.com
// ------------------------------------------------------------------------------------------------------------
//
// Dieses Beispiel hier läuft auf dem Board D071, D072, D073 von Display 3000

#include <stdio.h>
#include <avr/io.h>
//#include <avr/wdt.h>
#include <util/delay.h>
#include <math.h>
#include <avr/pgmspace.h>
//#include <avr/eeprom.h>
#include <string.h>
#include <stdbool.h>

#include "DISP_3000_driver.h"


const unsigned int LCD_Width  = MAX_X;
const unsigned int LCD_Height = MAX_Y;


EWindowOrientation Orientation = Landscape180;
EWindowOrientation ili_Orientation = Portrait180;



//-------------------------------------------------------------------------------
// Initialize the display
//-------------------------------------------------------------------------------
void LCD_Init(void)
{
	DDRB = 255; //all Ports to output
	SPSR        |= _BV(SPI2X);
	SPCR        = _BV (SPE) | _BV(MSTR);// | _BV(CPOL) | _BV(CPHA);
	
	_delay_ms(300);
	RESET(LCD_RESET);	_delay_ms(75);			// Set PB6 to false and wait
	SET(LCD_SELECT);	_delay_ms(75);			// Set PB2 to true and wait
	RESET(LCD_CLK);		_delay_ms(75); 			// Set PB7 to false and wait
	SET(LCD_DC);		_delay_ms(75); 			// Set PB4 to true and wait
	SET(LCD_RESET);		_delay_ms(75); 			// Set PB6 to true and wait
	
	unsigned int init_data[] =
	{
		0xFDFD, 0xFDFD,
		// pause
		0xEF00, 0xEE04, 0x1B04, 0xFEFE, 0xFEFE,
		0xEF90, 0x4A04, 0x7F3F, 0xEE04, 0x4306,
		// pause
		0xEF90, 0x0983, 0x0800, 0x0BAF, 0x0A00,
		0x0500, 0x0600, 0x0700, 0xEF00, 0xEE0C,
		0xEF90, 0x0080, 0xEFB0, 0x4902, 0xEF00,
		0x7F01, 0xE181, 0xE202, 0xE276, 0xE183,
		0x8001, 0xEF90, 0x0000
	};
	LCD_SPI_DBCommand(&init_data[0], 2);	_delay_ms(75);
	LCD_SPI_DBCommand(&init_data[2], 10);	_delay_ms(75);
	LCD_SPI_DBCommand(&init_data[12], 23);
	RESET(LCD_SELECT);
}

//-------------------------------------------------------------------------------
//Set Output window
//-------------------------------------------------------------------------------
void LCD_Window(int x1, int y1, int x2, int y2)
{

	unsigned char data[] =
	{
		0xEF, 0x08,
		0x18, 0x00,
		0x12, x1,
		0x15, x2,
		0x13, y1,
		0x16, y2
	};
	switch (Orientation)
	{
		default:
		// Invalid! Fall through to portrait mode
		case Portrait:
		// do nothing, data is intitialized for Portrait mode
		break;
		case Portrait180:
		data[3] = 0x03;		// Mode
		data[5] = LCD_Width-1 - x1;
		data[7] = LCD_Width-1 - x2;
		data[9] = LCD_Height-1 - y1;
		data[11] = LCD_Height-1 - y2;
		break;
		case Landscape:
		data[3] = 0x05;		// Mode
		data[5] = LCD_Width-1 - y1;
		data[7] = LCD_Width-1 - y2;
		data[9] = x1;
		data[11] = x2;
		break;
		case Landscape180:
		data[3] = 0x06;		// Mode
		data[5] = y1;
		data[7] = y2;
		data[9] = LCD_Height-1 - x1;
		data[11] = LCD_Height-1 - x2;
		break;
	}
	SET(LCD_DC);														// switch to command mode as we send a command array, not a data array
	RESET(LCD_SELECT);             										// enable display
	LCD_SPI_Byte(data, 12); 											// send 12 command bytes
	RESET(LCD_DC);														// disable command mode = data mode on
}

//-------------------------------------------------------------------------------
// Send single bytes (or array with Bytes) by SPI
//-------------------------------------------------------------------------------
void LCD_SPI_Byte(const unsigned char data[], unsigned int count)
{
	
	//....data
	for (unsigned int i=0; i<count; ++i)	// Count Bytes
	{
		SPCR |= _BV(SPE);
		SPDR = data[i];
		LCD_Wait();
	}
}

//-------------------------------------------------------------------------------
// Send word (16 bit) (or array with words) by SPI
//-------------------------------------------------------------------------------
void LCD_SPI_DBCommand(const unsigned int data[], unsigned int count)
{
	for (int i=0; i<count; ++i)
	{
		SET(LCD_DC);						// Selects Command mode
		RESET(LCD_SELECT);					// Enable display
		unsigned char msbyte = (data[i] >> 8) & 0xff;		// msb!
		unsigned char lsbyte = data[i] & 0xff;			// lsb!
		SPCR |= _BV(SPE); 					//SPI: Ready
		SPDR = msbyte;						//send first 8 bits
		LCD_Wait();							//Wait until all bits has been sent
		SPCR |= _BV(SPE);
		SPDR = lsbyte;						//send last 8 bits
		LCD_Wait();
		RESET(LCD_DC);						// Selects Data mode
		SET(LCD_SELECT);					// Disable display
	}
}

//-------------------------------------------------------------------------------
// Send integer by SPI
//-------------------------------------------------------------------------------
void LCD_SPI_Int(unsigned int Value)
{
	SPCR |= _BV(SPE);
	SPDR = (Value >> 8) & 0xff;
	LCD_Wait();
	SPCR |= _BV(SPE);
	SPDR = Value & 0xff;
	LCD_Wait();
}

//-------------------------------------------------------------------------------
// Wait for SPI transfer is done
//-------------------------------------------------------------------------------
void LCD_Wait(void)
{
	while (SPCR & _BV(SPE))
	{
		while (!(SPSR & (_BV(SPIF))));
		SPCR &= ~(_BV(SPE));
	}
}


void LCD_End_drawing(void){
	SET(LCD_SELECT);	// disable Chipselect
}


