/*
 * status.h
 *
 * Created: 09.07.2021 16:17:52
 *  Author: qfj
 */ 


#ifndef STATUS_H_
#define STATUS_H_

#include <stdint.h>

enum StatusBit
{
	NETWORK_ERROR,
	NO_REPLY_ERROR,
	STARTED_FILLING_ERROR,
	STOPPED_FILLING_ERROR,
	CHANGED_OPTIONS_ERROR,
	SLOW_TRANSMISSION_ERROR,
	LETTERS_ERROR,
	TEMPPRESS_ERROR,
	TIMER_ERROR,
	INIT_OFFLINE_ERROR,
	I2C_BUS_ERROR,
	VOLUME_TOO_BIG_ERROR
};



void SET_ERROR(enum StatusBit Bit);
void CLEAR_ERROR(enum StatusBit Bit);
_Bool CHECK_ERROR(enum StatusBit Bit);
void CLEAR_ALL(void);

void set_status_byte(uint16_t status_byte);
uint8_t get_status_byte(void);






#endif /* STATUS_H_ */