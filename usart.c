// Usart.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_USART 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on March 2016

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "xbee.h"
#include "xbee_utilities.h"
#include "usart.h"


UsartType USART0 = {.cmd_line = 0,.sending_cmd = 0};
UsartType USART1 = {.cmd_line = 0,.sending_cmd = 0};
	
uint8_t lastUSART1_Message[50];


// Initialize Interrupt Service Routine driven USART communication
void usart_init(uint16_t UBRR_register)
{
	UCSR0B = 0; // Added by JG to be sure...
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);   					//Turn on the transmission and reception circuitry
	// UCSR0B => USART Control and Status Register 0 B
	// TXEN0 => Transmitter Enable: Writing this bit to one enables the USART Transmitter. Details on datasheet - page 187
	// RXEN0 => Receiver Enable: Writing this bit to one enables the USART Receiver. Details on datasheet - page 187
	// RXCIE0 => RX Complete Interrupt Enable: Writing this bit to one enables interrupt on the RXCn Flag. A USART Receive Complete interrupt will be generated only if the RXCIEn bit is written to one, the Global Interrupt Flag in SREG is written to one and the RXCn bit in UCSRnA is set.
	// TXCIE0 => TX Complete Interrupt Enable: Writing this bit to one enables interrupt on the TXCn Flag. A USART Transmit Complete interrupt will be generated only if the TXCIEn bit is written to one, the Global Interrupt Flag in SREG is written to one and the TXCn bit in UCSRnA is set.
	
	UCSR0C = 0; // Added by JG to be sure...
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); 		// Use 8-bit character sizes
	
	// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0L = (unsigned char)UBRR_register;
	// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR0H = (unsigned char)(UBRR_register>>8);

	buffer_init();									//Init RingBuffer
}


// Initialize Interrupt Service Routine driven USART communication
void usart1_init(uint16_t UBRR_register)
{
	UCSR1B = 0; // Added by JG to be sure...
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1) | (1 << TXCIE1);   					//Turn on the transmission and reception circuitry
	// UCSR0B => USART Control and Status Register 0 B
	// TXEN0 => Transmitter Enable: Writing this bit to one enables the USART Transmitter. Details on datasheet - page 187
	// RXEN0 => Receiver Enable: Writing this bit to one enables the USART Receiver. Details on datasheet - page 187
	// RXCIE0 => RX Complete Interrupt Enable: Writing this bit to one enables interrupt on the RXCn Flag. A USART Receive Complete interrupt will be generated only if the RXCIEn bit is written to one, the Global Interrupt Flag in SREG is written to one and the RXCn bit in UCSRnA is set.
	// TXCIE0 => TX Complete Interrupt Enable: Writing this bit to one enables interrupt on the TXCn Flag. A USART Transmit Complete interrupt will be generated only if the TXCIEn bit is written to one, the Global Interrupt Flag in SREG is written to one and the TXCn bit in UCSRnA is set.
	
	UCSR1C = 0; // Added by JG to be sure...
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10); 		// Use 8-bit character sizes
	
	// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR1L = (unsigned char)UBRR_register;
	// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR1H = (unsigned char)(UBRR_register>>8);

	//buffer_init();									//Init RingBuffer
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



