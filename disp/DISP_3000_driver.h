/*
 * DISP_3000_driver.h
 *
 * Created: 10.05.2022 19:15:58
 *  Author: qfj
 */ 


#ifndef DISP_3000_DRIVER_H_
#define DISP_3000_DRIVER_H_





#define LCD_PORT 		PortB
#define LCD_CLK 		7		// Clock
#define LCD_RESET 		6		// Reset Display
#define LCD_SELECT 		2		// Cable Select
#define LCD_DC 			4		// Data/Command
#define LCD_DATA 		5		// Send Data

#define SET(bit) (PORTB |= _BV(bit))
#define RESET(bit) (PORTB &= ~_BV(bit))


typedef enum
{
	Portrait		= 0,	// Do not change these values!
	Portrait180		= 3,	// They are sent to the display
	Landscape		= 5,	// in order to set the graphics
	Landscape180	= 6		// mode of the display.
} EWindowOrientation;






//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

//Display Dimensions
#define MAX_X   132
#define MAX_Y   176

unsigned int max_x, max_y;




typedef enum
{
	cm256 = 0,
	cm65k = 1
} EColorMode;


typedef enum
{
	cm65k_uncompressed		= 0,
	cm65k_compressed		= 1,
	cm256low_uncompressed	= 2,
	cm256low_compressed		= 3,
	cm256high_uncompressed	= 4,
	cm256high_compressed	= 5
} ECompressionMode;



//Common Functions (Used as Interface for higher level Functions)
void LCD_Init(void);
void LCD_Window(int x1, int y1, int x2, int y2);
void LCD_SPI_Int(unsigned int Value);
void LCD_End_drawing(void);


//Display Sepecific Functions
void LCD_SPI_Byte(const unsigned char data[], unsigned int count);
void LCD_SPI_DBCommand(const unsigned int data[], unsigned int count);

void LCD_Wait(void);

#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define PIN(x)         GLUE(PIN, x)
#define DDR(x)         GLUE(DDR, x)

#define RGB(R,G,B) ((((R) & 0x1f) << 11) + (((G) & 0x3f) << 5) + ((B) & 0x1f))

#define THICK   1
#define THIN    0

#define FILL    1
#define NOFILL  0


#endif /* DISP_3000_DRIVER_H_ */