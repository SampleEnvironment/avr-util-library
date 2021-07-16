/*
 * status.c
 *
 * Created: 09.07.2021 16:17:39
 *  Author: qfj
 */ 
#include "status.h"

uint16_t status; 

void set_status_byte(uint16_t new_status){
	status = new_status;
}



uint16_t get_status(void){
	
	return  status;
}


void SET_ERROR(enum StatusBit Bit){
	status|=(1<<Bit);
}

void CLEAR_ERROR(enum StatusBit Bit){
	status&=~(1<<Bit);
}

_Bool CHECK_ERROR(enum StatusBit Bit){
	return status & (1<<Bit);
}

void CLEAR_ALL(void){
	status = 0;
}

uint8_t get_status_byte_levelmeter(void){
	
	
	uint8_t reduced_status = 0;

		if(CHECK_ERROR(NETWORK_ERROR))	reduced_status |= (1 << NETWORK_ERROR);
		if(CHECK_ERROR(NO_REPLY_ERROR))	reduced_status |= (1 << NO_REPLY_ERROR);
		if(CHECK_ERROR(STARTED_FILLING_ERROR))	reduced_status |= (1 << STARTED_FILLING_ERROR);
		if(CHECK_ERROR(STOPPED_FILLING_ERROR))	reduced_status |= (1 << STOPPED_FILLING_ERROR);
		if(CHECK_ERROR(CHANGED_OPTIONS_ERROR))	reduced_status |= (1 << CHANGED_OPTIONS_ERROR);
		if(CHECK_ERROR(SLOW_TRANSMISSION_ERROR))	reduced_status |= (1 << SLOW_TRANSMISSION_ERROR);
		if(CHECK_ERROR(LETTERS_ERROR))	reduced_status |= (1 << LETTERS_ERROR);
		
		return reduced_status;
}