/*
 * status.c
 *
 * Created: 09.07.2021 16:17:39
 *  Author: qfj
 */ 
#include "status.h"

uint16_t status; 
uint16_t status_reset_on_send;

void set_status_byte(uint16_t new_status){
	status = new_status;
	status_reset_on_send |= status;
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


// clears all network related errors
void CLEAR_ALL(void){
	CLEAR_ERROR(NETWORK_ERROR);
	CLEAR_ERROR(NO_REPLY_ERROR);
	CLEAR_ERROR(STARTED_FILLING_ERROR);
	CLEAR_ERROR(STOPPED_FILLING_ERROR);
	CLEAR_ERROR(CHANGED_OPTIONS_ERROR);
	CLEAR_ERROR(SLOW_TRANSMISSION_ERROR);
	//CLEAR_ERROR(LETTERS_ERROR);
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