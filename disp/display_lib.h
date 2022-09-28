/*
* display_lib.h
*
* Created: 11.05.2022 10:39:25
*  Author: qfj
*/


#ifndef DISPLAY_LIB_H_
#define DISPLAY_LIB_H_
#include "../../config.h"

// Display.h - Copyright 2016, HZB, ILL/SANE & ISIS

// Add the following commands:
//
// => ...

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
//
//
// Dieses Beispiel hier läuft auf dem Board D071, D072, D073 von Display 3000




/*************************************************************/
/**
*  @file
*  @defgroup display Basic Display Library
*  @author Peter Wegmann
*  @brief Basic Display Library
*
* This Display Library provides interfaces for drawing basic shapes and text on the connected display.
*
*
*/

/**@{*/



	/**
	* @brief Font dimensions
	*
	* Contains the width and height of a char as well as the width and height of a cell of a given font.
	*
	*/
	
	typedef struct
	{
		unsigned char CharWidth;	/**< @brief How many pixel wide (real pixel data, not including any empty pixel for frame)   */
		unsigned char CharHeight;	/**< @brief How many pixel height (real pixel data, not including any empty pixel for frame) */
		unsigned char CellWidth;	/**< @brief How many pixel wide including empty pixels, if you like to get more space between the characters raise this number */
		unsigned char CellHeight;	/**< @brief How many pixel height including empty pixels, if you like to get more space between the lines raise this number  */
	} TFontInfo;




	extern unsigned int WindowWidth(void);
	extern unsigned int WindowHeight(void);

	//extern LVM_ModelType LVM;


	// COLORS
	#define bright_blue		0b1101111011011111	//Predefined colors will make programmers life easier
	#define blue			0b0000000000011111
	#define dark_blue		0b0000000000010011
	#define bright_yellow	0b1111111111001100	//as the display uses 65.536 colors we have to define double-bytes for each color
	#define yellow			0b1111111111100000	//Check the programmers manual to learn how to define your own color
	#define orange			0b1111110011000110
	#define bright_red		0b1111100011100011
	#define red				0b1111100000000000
	#define dark_red		0b1001100000000000
	#define bright_green 	0b1001111111110011
	#define green 			0b0000011111100000
	#define dark_green 		0b0000001101100000
	#define white 			0b1111111111111111
	#define grey 			0b0011100011100111
	#define black 			0b0000000000000000

	#define HZB_Blue        0b0000001011010011
	#define HZB_Cyan        0b0000010011111100



	/**
	* @brief Print String
	*
	* Prints a string at given x y coordinates on the display. Font size of the String can be controlled by FontNr, XScale and YScale. Coloring is done by choosing Forecolor and BackColor accordingly.
	* When printing always the full Cell is drawn.
	*
	* @param Text ASCII encoded String that is printed
	* @param X
	* @param Y
	* @param FontNr Selects the Font (1: small; 2:medium; 3:large)
	* @param XScale scaling in x direction
	* @param YScale scaling in y direction
	* @param ForeColor Color of the text
	* @param BackColor Background color (cell)
	*
	* @return void
	*/
	void LCD_Print(const char* Text, uint16_t X, uint16_t Y, unsigned char FontNr, unsigned char XScale, unsigned char YScale, unsigned int ForeColor, unsigned int BackColor);


	/**
	* @brief Print Logo
	*
	* Prints the HZB Logo at given x and y coordinates on the display. The pixel data for the logo loaded from a bitmap.
	* @param x
	* @param y
	* @param BackColor The Background color of the Logo
	*
	* @return void
	*/
	void LCD_LOGO(uint16_t x, uint16_t y,uint16_t BackColor);

	/**
	* @brief Clear screen
	*
	*	Clears the LCD by plotting agiven color on the whole screen
	*
	* @param color
	*
	* @return void
	*/
	void LCD_Cls(unsigned int color);


	/**
	* @brief Plot Pixel
	*
	*	Plots a Pixel on the screen
	*
	* @param x1
	* @param y1
	* @param line_type if set to THICK 2x2 Pixels a plotted
	* @param color color of the Pixel
	*
	* @return void
	*/
	void LCD_Plot(uint16_t x1, uint16_t y1, unsigned char line_type, unsigned int color);



	/**
	* @brief Plot Box
	*
	*	Plots a colored box that is defined by the two points (x1,y1) and (x2,y2)
	*
	*
	* @param x1
	* @param y1
	* @param x2
	* @param y2
	* @param color
	*
	* @return void
	*/
	void LCD_Box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned int color);



	/**
	* @brief Plot Rectangle
	*
	*	Plots a colored rectangle that is defined by the two points (x1;y1) and (x2;y2). Line thickness can be selected by line_type.
	*
	* @param x1
	* @param y1
	* @param x2
	* @param y2
	* @param line_type selects line thickness THICK: 2  or THIN: 1
	* @param color
	*
	* @return void
	*/
	void LCD_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, unsigned char line_type, unsigned int color);



	/**
	* @brief Draws a line from point (x1,y1) to Point (x2,y2)
	*
	* @param x1
	* @param y1
	* @param x2
	* @param y2
	* @param line_type  selects line thickness THICK: 2  or THIN: 1
	* @param color
	*
	* @return void
	*/
	void LCD_Draw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,unsigned char line_type ,unsigned int color);



	_Bool LCD_Dialog(char *title, char *text, unsigned int BackColor, unsigned int ForeColor,uint8_t timeout);


	void LCD_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);
	void LCD_vline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);

	void LCD_Draw_Cross(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
	void LCD_conn_Stregth(uint8_t NoNetw,int8_t RSSI,uint16_t x, uint16_t y, uint16_t color);

	#define THICK   1
	#define THIN    0

	#define FILL    1
	#define NOFILL  0


	
	#ifdef LEVELMETER
	
	#ifdef ili9341
	#define LOGO_BIG
	
	#define BOX_W_SIGSTRENGTH 12
	#define BOX_H_SIGSTRENGTH 24
	
	#define X0_SiGSTRENTH 2
	#define Y0_SiGSTRENTH 18

	#define BAR_W_SIGSTRENGTH 2
	#define BAR_SPACEING_SIGSTRENGTH 2
	#define BAR_H_SIGSTRENGTH 3
	#define BAR_MIN_H_SIGSTRENGTH 3
	#define NOW_NETW_BAR_POS_SIGSTENGTH 11

	
	#endif // ili9341
	
	#ifdef DISP_3000
	#define LOGO_SMALL
		#define BOX_W_SIGSTRENGTH 6
		#define BOX_H_SIGSTRENGTH 9
		
		#define X0_SiGSTRENTH 4
		#define Y0_SiGSTRENTH 7

		#define BAR_W_SIGSTRENGTH 0
		#define BAR_SPACEING_SIGSTRENGTH 2
		#define BAR_H_SIGSTRENGTH 2
		#define BAR_MIN_H_SIGSTRENGTH 1
		#define NOW_NETW_BAR_POS_SIGSTENGTH 4
	#endif // DISP_3000
	
	#endif // LEVELMETER

	#ifdef GASCOUNTER_MODULE
	#ifdef ili9341
	#define LOGO_SMALL
	#define BOX_W_SIGSTRENGTH 12
	#define BOX_H_SIGSTRENGTH 24
	
	#define X0_SiGSTRENTH 2
	#define Y0_SiGSTRENTH 18

	#define BAR_W_SIGSTRENGTH 2
	#define BAR_SPACEING_SIGSTRENGTH 2
	#define BAR_H_SIGSTRENGTH 3
	#define BAR_MIN_H_SIGSTRENGTH 3
	#define NOW_NETW_BAR_POS_SIGSTENGTH 11
	
	#endif // ili9341
	
	#ifdef GCM_old_disp
	#define LOGO_SMALL
	
		#define LOGO_SMALL
		#define BOX_W_SIGSTRENGTH 12
		#define BOX_H_SIGSTRENGTH 24
		
		#define X0_SiGSTRENTH 2
		#define Y0_SiGSTRENTH 18

		#define BAR_W_SIGSTRENGTH 2
		#define BAR_SPACEING_SIGSTRENGTH 2
		#define BAR_H_SIGSTRENGTH 3
		#define BAR_MIN_H_SIGSTRENGTH 3
		#define NOW_NETW_BAR_POS_SIGSTENGTH 11
#endif
	#endif // GASCOUNTER_MODULE
	/*
	#define LOGO_SMALL
	*/


	#endif /* DISPLAY_LIB_H_ */