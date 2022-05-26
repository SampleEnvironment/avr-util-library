/*
* PinAllocation.h
*
* Created: 11.05.2022 19:11:09
*  Author: qfj
*/


#ifndef PINALLOCATION_H_
#define PINALLOCATION_H_

#ifdef LEVELMETER

#ifdef ili9341
#define LCD_CLK 		PINB7		// Clock
#define LCD_RESET 		PINB6		// Reset Display
#define LCD_SELECT 		PINB2		// Cable Select
#define LCD_DC 			PINB4		// Data/Command
#define LCD_MOSI 		PINB5		// Send Data
#define LCD_LED         PINB3	    // LCD Backlight
#define LCD_PORT PORTB
#define LCD_LED_PORT PORTB
#define LCD_DDRB  255
#endif

#ifdef DISP_3000
#define LCD_CLK 		PINB7		// Clock
#define LCD_RESET 		PINB6		// Reset Display
#define LCD_SELECT 		PINB2		// Cable Select
#define LCD_DC 			PINB4		// Data/Command
#define LCD_MOSI 		PINB5		// Send Data
#define LCD_LED         PINB3 //PIND6
#define LCD_PORT PORTB
#define LCD_LED_PORT PORTB

#endif


#endif // LEVELMETER

#ifdef GASCOUNTER_MODULE

#ifdef ili9341
#define LCD_CLK 		PINB7		// Clock
#define LCD_RESET 		PINB6		// Reset Display
#define LCD_SELECT 		PINB3		// Cable Select
#define LCD_DC 			PINB4		// Data/Command
#define LCD_MOSI 		PINB5		// Send Data
#define LCD_LED         PIND6		// LCD Backlight
#define LCD_PORT PORTB
#define LCD_LED_PORT PORTD
#define LCD_DDRB  248
#endif

#ifdef GCM_old_disp
#define LCD_CLK 		PINB3		// Clock
#define LCD_RESET 		PINB6		// Reset Display
#define LCD_SELECT 		PINB7		// Cable Select
#define LCD_DC 			PINB5		// Data/Command
#define LCD_MOSI 		PINB4		// Send Data
#define LCD_LED         PINB3 //PIND6
#define LCD_PORT PORTB
#define LCD_LED_PORT PORTB

#endif

#endif // GASCOUNTER_MODULE


#endif /* PINALLOCATION_H_ */