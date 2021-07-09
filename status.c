/*
 * status.c
 *
 * Created: 09.07.2021 16:17:39
 *  Author: qfj
 */ 
#include "status.h"

uint16_t status; 

void set_status(uint16_t new_status){
	status = new_status;
}

uint16_t get_status(void){
	return status;
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