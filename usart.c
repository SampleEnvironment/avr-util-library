// Usart.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_USART 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on March 2016

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "../config.h"

#include "xbee.h"
#include "xbee_utilities.h"
#include "usart.h"


UsartType USART0 = {.cmd_line = 0,.sending_cmd = 0};
UsartType USART1 = {.cmd_line = 0,.sending_cmd = 0};
	
uint8_t lastUSART1_Message[50];


// Initialize Interrupt Service Routine driven USART communication
void usart_init(void)
{
	UCSR0B = 0;
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);

	UCSR0C = 0;
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	uint16_t ubrr = UBRR_VAL(USART_BAUDRATE);
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)(ubrr);

	buffer_init();
}

void usart1_init(void)
{
	UCSR1B = 0;
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1) | (1 << TXCIE1);

	UCSR1C = 0;
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10);

	uint16_t ubrr = UBRR_VAL(USART1_BAUDRATE);
	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)(ubrr);
}

// USART0 data received interrupt service routine (from XBee module)
ISR(USART0_RX_vect) 
{	
	static uint16_t length_counter = 0;
	static uint16_t summe = 0;
	static uint16_t frame_length = 0;
	static uint8_t buffer[SINGLE_FRAME_LENGTH];
	static uint8_t buffer_size = 0;
	
	uint8_t data = USART_IODR;					// Read USART Input Data Register

	if(buffer_size >= SINGLE_FRAME_LENGTH-1)
	{
		xbee_build_frame(buffer, buffer_size);
		buffer_size=0;
		frame_length=0;
	}
	if(USART0.cmd_line) return;
	
	if (length_counter > 1)
	{
		length_counter--;
		summe=+data;
		buffer[buffer_size++] = data;
	}
			
	if (frame_length > 1)
	{
		frame_length--;
		buffer[buffer_size++] = data;
	}
		
	if (frame_length == 1) 
	{
		frame_length--;
		xbee_build_frame(buffer, buffer_size);
		buffer_size = 0;
		return;
	}
	
	if (length_counter == 1) 
	{
		frame_length = summe+2;
		length_counter--;
	}
	
	if (data == 0x7E)
	{						 							
		if(!frame_length)
		{
			length_counter = 3;
			buffer[buffer_size++] = data;
		}
	}
}


// USART1 data received interrupt service routine (from XBee module)
ISR(USART1_RX_vect)
{

	static uint8_t buffer[50];
	static uint8_t buffer_size = 0;
	
	uint8_t data = USART_IODR1	;					// Read USART Input Data Register

	if((buffer_size >= 50-2)||(data == '\r'))
	{	
		buffer[buffer_size++] = '\0';
		memcpy(lastUSART1_Message, buffer,buffer_size);
		buffer_size=0;
		return;
	}
	
	buffer[buffer_size++] = data;
	

}

// USART0 data transmitted interrupt service routine (to XBee module)
ISR(USART0_TX_vect) 
{
	if(USART0.sending_cmd) 
	{	
		USART_IODR = *USART0.send_str_reader++;		// Write USART0 Input Data Register
		USART0.sending_cmd--;
	}
}


// USART0 data transmitted interrupt service routine (to XBee module)
ISR(USART1_TX_vect)
{
	if(USART1.sending_cmd)
	{
		USART_IODR1 = *USART1.send_str_reader++;		// Write USART0 Input Data Register
		USART1.sending_cmd--;
	}
}



