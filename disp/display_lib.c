/*
* display_lib.c
*
* Created: 11.05.2022 10:39:47
*  Author: qfj
*/


//
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
#include <util/delay.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdbool.h>

#include "display_lib.h"

#ifdef ili9341
#include "ili9341_driver.h"
#endif // ili9341

#ifdef DISP_3000
#include "DISP_3000_driver.h"
#endif

#ifdef GCM_old_disp  //TODO --> more elegant solution needed not really needed only for compilation
#include "ili9341_driver.h"
#endif

#include "fontConfig.h"


extern EWindowOrientation Orientation;


unsigned int WindowWidth(void)
{
	if (Orientation==Portrait || Orientation==Portrait180)
	return MAX_X;
	else
	return MAX_Y;
}

unsigned int WindowHeight(void)
{
	if (Orientation==Portrait || Orientation==Portrait180)
	return MAX_Y;
	else
	return MAX_X;
}

struct Logo
{
	const uint16_t w ;
	const uint16_t h ;
	const uint16_t data_len;
	const uint8_t bytes_per_line;
	const uint8_t h_Blue;
};





#ifdef LOGO_SMALL

struct Logo HZB_logo = {.w = 160,.h = 56, .data_len = 1120,.bytes_per_line = 20,.h_Blue = 37};
extern const uint8_t HZB_LOGO_DISP_3000[] PROGMEM;
#endif

#ifdef LOGO_BIG
struct Logo HZB_logo = {.w = 290,.h = 102, .data_len = 3774,.bytes_per_line = 37,.h_Blue = 65};
extern const uint8_t HZB_LOGO_ili9341[] PROGMEM;
#endif


#ifdef ili9341





extern const uint8_t Font3[],Font4[],Font5[] PROGMEM;	// Shared font arrays stored in Program-Memory

TFontInfo FontInfo[3] =
{
	{ 8, 16,  8, 16}, // normal font
	{12, 24, 12, 24}, // large font
	{FONT3_W, FONT3_H, FONT3_W, FONT3_H}  // He-Level font (extra large)
};

#endif

#ifdef DISP_3000




extern const uint8_t Font1[], Font2[] PROGMEM;	// Shared font arrays stored in Program-Memory

TFontInfo FontInfo[2] =
{
	{ 5,  8, 6,  9},		// normal font
	{ 8, 14, 9, 15}	// large font
	
};


#endif




// ##### LCD subroutines below  ######################################################################################





#ifdef ili9341


void LCD_Print(const char* Text, uint16_t X, uint16_t Y, unsigned char FontNr,
unsigned char XScale, unsigned char YScale, unsigned int ForeColor, unsigned int BackColor){
	/*
	if (--FontNr > 1) FontNr = 1;
	const uint8_t* Font PROGMEM = (FontNr==0) ? Font3 : Font4;
	*/

	const uint8_t* Font ;//PROGMEM;
	
	switch (FontNr){
		case 1:  Font = Font3; break;
		case 2:  Font = Font4; break;
		case 3:  Font = Font5; break;
		default: Font = Font3; break;
	}
	--FontNr;
	const unsigned int Len = strlen(Text);
	const unsigned int CharWidth = FontInfo[FontNr].CharWidth;
	const unsigned int CharHeight = FontInfo[FontNr].CharHeight;
	const uint16_t BytePerChar = (CharHeight*CharWidth)/8; // number of Byte per line in Font Table
	const uint8_t BytesPerVertLine = CharHeight/8;//1 + ((CharHeight - 1) / 8); // Rounding Up Integer divisin only would be floor(Charheight/8)
	const uint8_t Scale = XScale;
	const uint8_t ScaleArea =  Scale * Scale;
	
	
	for (uint16_t LetterIndex = 0;LetterIndex<Len;LetterIndex++){
		unsigned char Ch = Text[LetterIndex];				// Ch is the position of character data in the font table
		if (Ch > 125)								// At the end of the font data some german special characters are defined
		{
			switch (Ch)								// special treatment eliminates the storage of unused data
			{
				case 228: Ch = 126; break;			// ä is at Pos. 127 etc.
				case 246: Ch = 127; break;			// ö
				case 252: Ch = 128; break;			// ü
				case 196: Ch = 129; break;			// Ä
				case 214: Ch = 130; break;			// Ö
				case 220: Ch = 131; break;			// Ü
				case 223: Ch = 132; break;			// ß
				case 176: Ch = 133; break;			// °
				case 179: Ch = 134; break;			// ³
				default: Ch = '?'; break;				// not allowed: change to ?
			}
		}
		// remapping for Font3 SPACE,1,2,...,9,A,B,...,X,Y,Z
		if (FontNr == 2)
		{
			if(Ch == 32 ){
				Ch -= 32;
			}
			if (Ch > 47 && Ch < 58)
			{
				Ch -= 47;
			}
			if (Ch > 64 && Ch < 91)
			{
				Ch -= 54;
			}
			
		}
		else
		{
			Ch -= 32;
		}
		
		
		const uint16_t BytePos = Ch * BytePerChar; //determines Line in Font Table
		
		//uint8_t VertlineBMP[3]={0};

		for(uint8_t vertline = 0;vertline <CharWidth;vertline++){
			
			LCD_Window(X, Y, X+Scale-1, Y+(CharHeight*Scale));// Set character window
			for (uint8_t i =0;i<BytesPerVertLine;i++){
				uint8_t Byte = pgm_read_byte(&Font[BytePos+i+vertline*BytesPerVertLine]);
				
				
				for (int Charbit=0; Charbit<=7; ++Charbit)	// check each bit of this character line
				{
					const int bShowPixel = (Byte >> Charbit) & 0x01;	// check if pixel is on or off (select appropriate bit)
					const unsigned int Color = bShowPixel ?
					ForeColor :										// pixel = on: foreground color
					BackColor;										// pixel = off: background color
					//_delay_ms(100);
					for(uint8_t i = 0;i<ScaleArea;i++){
						LCD_SPI_Int(Color);								// draw pixel
					}
				}										// check next bit of this character
				
			}
			
			LCD_End_drawing();
			// next row of this character
			X+= Scale;
		}
		if (FontNr == 2)
		{
			X -= 10;
		}

	}


}

#endif
#ifdef DISP_3000

//-------------------------------------------------------------------------------
// Displays a string
//-------------------------------------------------------------------------------
void LCD_Print(const char* Text, uint16_t X, uint16_t Y, unsigned char FontNr,
unsigned char XScale, unsigned char YScale, unsigned int ForeColor, unsigned int BackColor)
{

	
	if (--FontNr > 1) FontNr = 1;
	const uint8_t* Font = (FontNr==0) ? Font1 : Font2;
	//	const prog_uint8_t* Font = (FontNr==0) ? Font1 : Font2;
	//	const uint8_t* Font = (FontNr==0) ? Font1 : Font2;
	const unsigned int Len = strlen(Text);
	const unsigned int CharWidth = FontInfo[FontNr].CharWidth;
	const unsigned int CharHeight = FontInfo[FontNr].CharHeight;
	const unsigned int CellWidth = FontInfo[FontNr].CellWidth;
	const unsigned int CellHeight = FontInfo[FontNr].CellHeight;

	for (unsigned int Index=0; Index<Len; ++Index)	// For each character in the string
	{
		unsigned char Ch = Text[Index];				// Ch is the position of character data in the font table
		if (Ch > 122)								// At the end of the font data some german special characters are defined
		{
			switch (Ch)								// special treatment eliminates the storage of unused data
			{
				case 228: Ch = 127; break;			// ä is at Pos. 127 etc.
				case 246: Ch = 128; break;			// ö
				case 252: Ch = 129; break;			// ü
				case 196: Ch = 130; break;			// Ä
				case 214: Ch = 131; break;			// Ö
				case 220: Ch = 132; break;			// Ü
				case 223: Ch = 133; break;			// ß
				default: Ch = '?'; break;				// not allowed: change to ?
			}
		}
		Ch -= 32;													// Substracts 32 as we are not using any character below ASCII 32, we are starting with the character space (ASCII 32)at the font table
		const unsigned int BytePos = Ch * CharHeight;				// Multiply with the data size of this font (our two fonts use either 5 or 8 data/pixel per line)

		LCD_Window(X, Y, X+CellWidth*XScale-1, Y+CellHeight*YScale+1);// Set character window

		for (unsigned int Loop=0; Loop<=CellHeight; ++Loop)			// For each vertical row of this character
		{
			uint8_t Byte  = pgm_read_byte(&Font[BytePos+Loop]);
			//			prog_uint8_t Byte = pgm_read_byte (&Font[BytePos+Loop]);	// fetch byte value of the character's row
			//			uint8_t Byte = pgm_read_byte (&Font[BytePos+Loop]);	// fetch byte value of the character's row
			if (Loop >= CharHeight) Byte = 0;							// the lines with background color at the lower end of the character
			for (unsigned int Pixel=1; Pixel<=YScale; ++Pixel)		// As often as needed by the Y-scale factor (e.g. draws complete line twice if double height font is requested)
			{
				const int XPixelTmp = (CellWidth-CharWidth)*XScale;	// how many empty columns are requested for this font
				for (int Charbit=1; Charbit<=XPixelTmp; ++Charbit)
				{
					// empty column = always background color
					LCD_SPI_Int(BackColor);								// draw pixel
				}

				for (int Charbit=CharWidth-1; Charbit>=0; --Charbit)	// check each bit of this character line
				{
					const int bShowPixel = (Byte >> Charbit) & 0x01;	// check if pixel is on or off (select appropriate bit)
					const unsigned int Color = bShowPixel ?
					ForeColor :										// pixel = on: foreground color
					BackColor;										// pixel = off: background color
					for (unsigned int C=1; C<=XScale; ++C)			// as often as needed by the horizotal scale factor (e.g. draws pixel twice if double wide font is requested)
					LCD_SPI_Int(Color);								// draw pixel
				}										// check next bit of this character
			}											// draw this line again if needed for scaling
		}
		LCD_End_drawing();
		// next row of this character
		
		X += CellWidth*XScale;
	}													// next character of this string
}

#endif


void LCD_LOGO(uint16_t x, uint16_t y,uint16_t BackColor){

	uint16_t ForeColor = HZB_Blue;



	LCD_Window(x, y, x+HZB_logo.w-1, y+HZB_logo.h);// Set character window

	uint16_t curr_Pixel = 0;
	
	for (uint16_t arr_index = 0; arr_index < HZB_logo.data_len; arr_index++)
	{

		#ifdef LOGO_BIG
		uint8_t Byte  = pgm_read_byte(&HZB_LOGO_ili9341[arr_index]);
		#endif // LOGO_BIG

		#ifdef LOGO_SMALL
		uint8_t Byte = pgm_read_byte(&HZB_LOGO_DISP_3000[arr_index]);
		#endif // LOGO_SMALL

		


		for (int BMPbit=7; BMPbit >= 0; --BMPbit)	// check each bit of this  line
		{
			if ((curr_Pixel > HZB_logo.w / 2) || (arr_index/HZB_logo.bytes_per_line > HZB_logo.h_Blue))
			{
				ForeColor = HZB_Cyan;
			}else
			{
				ForeColor = HZB_Blue;
			}
			
			const uint8_t bShowPixel = (Byte >> BMPbit) & 0x01;	// check if pixel is on or off (select appropriate bit)
			const uint16_t Color = bShowPixel ?
			ForeColor :										// pixel = on: foreground color
			BackColor;										// pixel = off: background color
			
			LCD_SPI_Int(Color);								// draw pixel
			curr_Pixel++;

			
			if (curr_Pixel >= HZB_logo.w)
			{
				curr_Pixel = 0;
				break;
			}
			

		}

	}

	LCD_End_drawing();


}

//-------------------------------------------------------------------------------
//Clear display
//-------------------------------------------------------------------------------

void LCD_Cls(unsigned int color)
{
	for(unsigned int i = 0; i<=WindowHeight();i++){
		LCD_hline(0,i,WindowWidth()+1,color);
	}
	

}

//-------------------------------------------------------------------------------
// Plot one pixel to the display
//-------------------------------------------------------------------------------
void LCD_Plot(uint16_t x1, uint16_t y1, unsigned char line_type, unsigned int color)
{

	if (line_type == THICK)
	{
		LCD_Window(x1, y1, x1+1, y1+1); 									// Define Window, exactly four pixels large
		LCD_SPI_Int(color);													// send four pixels (avoid loop overhead)
		LCD_SPI_Int(color);
		LCD_SPI_Int(color);
		LCD_SPI_Int(color);
	}
	else
	{
		LCD_Window(x1, y1, x1, y1); 										// Define Window, exactly one pixel large
		LCD_SPI_Int(color);													// send pixel
	}
	LCD_End_drawing();
}

//-------------------------------------------------------------------------------
// Draw a rectangular filled box
//-------------------------------------------------------------------------------
void LCD_Box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned int color)
{

	LCD_Window(x1, y1, x2, y2);   										// Define Window
	for (unsigned int i=0; i<((x2-x1+1)*(y2-y1+1)); ++i)			// for every pixel...
	LCD_SPI_Int(color); 											// ...send color information
	LCD_End_drawing();
}

//-------------------------------------------------------------------------------
// Draw a rectangular (not filled)
//-------------------------------------------------------------------------------
void LCD_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned char line_type, unsigned int color)	//draw rectangle
{
	/*
	LCD_hline(x1, y1,  x2-x1, color);
	LCD_hline(x1, y2,  x2-x1, color);
	LCD_vline(x1, y1,  x2-x1, color);
	LCD_vline(x2, y1,  x2-x1, color);
	*/
	LCD_Draw(x1, y1, x2, y1, line_type, color);
	LCD_Draw(x2, y1, x2, y2, line_type, color);
	LCD_Draw(x1, y2, x2, y2, line_type, color);
	LCD_Draw(x1, y1, x1, y2, line_type, color);
}

//-------------------------------------------------------------------------------
// Draw a line
//-------------------------------------------------------------------------------
void LCD_Draw(uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, unsigned char line_type, unsigned int color)//draw line - bresenham algorithm
{
	


	int x = x1;
	int y = y1;
	int d = 0;
	int hx = x2 - x1;    									// how many pixels on each axis
	int hy = y2 - y1;
	int xinc = 1;
	int yinc = 1;
	if (hx < 0)
	{
		xinc = -1;
		hx = -hx;
	}
	if (hy < 0)
	{
		yinc = -1;
		hy = -hy;
	}
	if (hy <= hx)
	{
		int c = 2 * hx;
		int m = 2 * hy;
		while (x != x2)
		{
			LCD_Plot(x, y, line_type, color);
			x += xinc;
			d += m;
			if (d > hx)
			{
				y += yinc;
				d -= c;
			}
		}
	}
	else
	{
		int c = 2 * hy;
		int m = 2 * hx;
		while (y != y2)
		{
			LCD_Plot(x, y, line_type, color);
			y += yinc;
			d += m;
			if (d > hy)
			{
				x += xinc;
				d -= c;
			}
		}
	}
	LCD_Plot(x, y, line_type, color); 												  // finally, the last pixel
}



void LCD_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color)
{
	
	
	LCD_Window(x0,y0,x0+length,y0);
	for(unsigned int i=0; i<length; i++){
		LCD_SPI_Int(color);
	}
	LCD_End_drawing();
}


void LCD_vline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color)
{
	

	
	LCD_Window(x0,y0,x0,y0+length);
	for(unsigned int i=0; i<length; i++){
		LCD_SPI_Int(color);
	}
	LCD_End_drawing();
}




void LCD_Draw_Cross(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
	LCD_Draw(x0,y0,x1,y1,0,red);
	LCD_Draw(x0,y1,x1,y0,0,red);
}

void LCD_conn_Stregth(uint8_t NoNetw,int8_t RSSI,uint16_t x, uint16_t y, uint16_t color){
		
	LCD_Box(x,y,x+FONT2_W*2,y+FONT2_H,black);
	
	uint16_t colors [5][4]= {
		{grey, grey, grey, grey},
		{color,grey, grey, grey},
		{color,color,grey, grey},
		{color,color,color,grey},
		{color,color,color,color}			
	};

	uint8_t k = 0;
	

	if(NoNetw){
		k = 0;
	}else{
		if (RSSI <= 40)
		{
			k = 4;

		}
		if (RSSI > 40)
		{
			k = 3;
		}
		if (RSSI > 50)
		{
			k = 2;
		}
		if (RSSI > 60)
		{
			k = 1;
		}
	}
	 
	
	
	uint16_t x0  = 2;
	uint16_t y0  = 18;
	
	uint16_t bar_w = 2;
	
	uint16_t bar_spacing = 2;
	
	uint16_t bar_h = 3;
	uint16_t bar_min_h = 3;
	
	for (uint16_t i=0;i <4; i++)
	{
		uint16_t offset = (bar_w + bar_spacing) *i;
		
		uint16_t yoffs = (bar_h * i);
		
		LCD_Box(x+x0+offset,y+y0-(bar_min_h+yoffs),x+x0+bar_w+offset,y+y0,colors[k][i]);
	}
	 
	

}




// ------------------------------------------------------------------------------------------------------
// History
// V1.03: 07.05.2008: speed up of filled circle by optimizing the code
// V1.03: 07.05.2008: Corrected the status of the "CS" line which could cause a problem when using several devices on the SPI bus
// V1.02: 29.04.2008: optimized some of the SPI line settings
// V1.01: first official version of Library


