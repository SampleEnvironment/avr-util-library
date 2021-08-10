// Usart.h - Copyright 2016, HZB, ILL/SANE & ISIS

#ifndef USART_H
#define USART_H

#include <avr/interrupt.h>

//RS232 USART constants
#define USART_BAUDRATE 	9600			// Define USART0 baudrate
#define USART_IODR 		UDR0			// Define USART0 I/O Data Register
#define USART_IODR1 	UDR1			// Define USART1 I/O Data Register


typedef struct{
	volatile uint8_t cmd_line;				// USART receive block
	volatile uint8_t *send_str_reader;		// Pointer to the next byte to send via USART
	volatile uint8_t sending_cmd;			// Number of bytes to send via USART
}UsartType;


//==============================================================
// USART commands
//==============================================================

void usart_init(uint16_t UBRR_register);			// Initialize Interrupt Service Routine driven USART communication USART0 --> xbee
void usart1_init(uint16_t UBRR_register);			// Initialize Interrupt Service Routine driven USART communication USART1 --> other peripherals
ISR(USART0_RX_vect); 			// USART0 data received interrupt service routine (from XBee module)
ISR(USART0_TX_vect); 			// USART0 data transmitted interrupt service routine (to XBee module)

#endif  // usart.h
