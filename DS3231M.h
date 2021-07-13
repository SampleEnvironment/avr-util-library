/**
*	@file 		DS3231M.h
*	@brief 		I2C Real-Time Clock header
*	@author		Klaus Kiefer, Peter Wegmann
*	@version	1.4
*	@date		2020/03/25
*
*
*/

#ifndef DS3231M_H_
#define DS3231M_H_


#include <stdint.h>
#include <time.h>




// I2C address of the DS3231M
#define DS3231M_address						0xD0 

// addresses of the DS3231M
#define DS3231M_address_seconds				0x00
#define DS3231M_address_minutes				0x01
#define DS3231M_address_hours				0x02
#define DS3231M_address_days				0x03
#define DS3231M_address_date				0x04
#define DS3231M_address_month				0x05
#define DS3231M_address_year				0x06
#define DS3231M_address_alarm1_seconds		0x07
#define DS3231M_address_alarm1_minutes		0x08
#define DS3231M_address_alarm1_hours		0x09
#define DS3231M_address_alarm1_days_date	0x0A
#define DS3231M_address_alarm2_minutes		0x0B
#define DS3231M_address_alarm2_hours		0x0C
#define DS3231M_address_alarm2_days_date	0x0D
#define DS3231M_address_control_reg			0x0E
#define DS3231M_address_status_reg			0x0F
#define DS3231M_address_aging_offset_reg	0x10
#define DS3231M_address_temp_MSB			0x11
#define DS3231M_address_temp_LSB			0x12

// bits in the control register // use disjunction to combine
#define DS3231M_control_reg_A1IE			0x01 // bit 0 LSB
#define DS3231M_control_reg_A2IE			0x02 // bit 1
#define DS3231M_control_reg_INTCN			0x04 // bit 2
#define DS3231M_control_reg_CONV			0x20 // bit 5
#define DS3231M_control_reg_BBSQW			0x40 // bit 6
#define DS3231M_control_reg_not_EOSC		0x80 // bit 7 MSB

// bits in the status register // use disjunction to combine
#define DS3231M_status_reg_EN32KHZ			0x08 // bit 3
#define DS3231M_status_reg_OSF				0x80 // bit 7


#define INRANGE(INPUT,MIN,MAX)  ((INPUT < MIN || INPUT > MAX) ? (0) : (1))

extern struct tm Time;
extern time_t last_Valid_Time;

uint8_t init_DS3231M(void (*printInfoFun)(char *,_Bool));
uint8_t DS3231M_read_register(uint8_t register_address);
void DS3231M_read_time(void);
void DS3231M_set_time(struct tm *newtime);
void DS3231M_write_register(uint8_t register_address, uint8_t data);
uint8_t check_valid_ranges(struct tm * newtime);

#endif 