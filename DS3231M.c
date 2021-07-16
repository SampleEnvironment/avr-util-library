/**
*	@file 		DS3231M.c
*	@brief 		I2C Real-Time Clock API
*	@author		Klaus Kiefer, Peter Wegmann
*	@version	1.4
*	@date		2020/03/25
*
*	A detailed documentation of the timing chip can be found in the <a href="DS3231M Datasheet.pdf" target="_blank"><b>DS3231M Datasheet</b></a>.

*/



#include <string.h>
#include <time.h>
#include <stdbool.h>


#include "DS3231M.h"
#include "status.h"


#ifndef UNIT_TEST
#include "I2C_utilities.h"
#include <util/eu_dst.h>
#else

#include "TestMocks/Mock_I2C_utilities.h"
#include "TestMocks/Mock_main.h"
#include "TestMocks/Mock_LCD.h"


#endif

/**
* @brief Time and Date
*
*
* Holds current Time (seconds, minutes and hours), and Date (days, months, and year)
*/
struct tm Time;


int8_t temp_MSB;
uint8_t temp_LSB;


/**
* @brief Holds Function to print Infoline 
*
* Paints an info message on the LCD 
*/
void (*print_info_DS3231M)(char * ,_Bool ) = NULL;





/**
* @brief  Initialization of the external device DS3231M, a time holding unit. It's connected via I2C (Two Wire Interface).
*
*
* @return uint8_t  0 on success and greater zero if initialization was unsuccessful
*
*/
uint8_t init_DS3231M(void (*printInfoFun)(char *,_Bool))
{
	print_info_DS3231M = printInfoFun;

	Time.tm_sec = 0;
	Time.tm_min = 0;
	Time.tm_hour = 0;
	Time.tm_mday = 0;
	Time.tm_mon = 0;
	Time.tm_year = 0;
	Time.tm_isdst =0;
	
	

	
	uint8_t data[2];
	
	
	
	if (I2C_read_from(DS3231M_address,DS3231M_address_temp_MSB,data,2))
	{
		return 1;
		
	}
	
	uint16_t Temp = ((uint16_t) data[0] << 8) | data[1];
	if(Temp == 0){
		return 1 ;
	}
	
	I2C_read_from(DS3231M_address,DS3231M_address_control_reg,data,1);
	
	uint8_t contr_reg = data[0];
	
	data[0] =  contr_reg | DS3231M_control_reg_CONV ;
	I2C_write_to(DS3231M_address,DS3231M_address_control_reg,data,1);
	
	return 0;
}



uint8_t check_valid_ranges(struct tm * newtime){
	if (
		INRANGE(newtime->tm_sec,0,59) &&
		INRANGE(newtime->tm_min,0,59) &&
		INRANGE(newtime->tm_hour,0,23) &&
		INRANGE(newtime->tm_mday,1,31) &&
		INRANGE(newtime->tm_mon,1,12) &&
		INRANGE(newtime->tm_year,0,99))
	{
		return 1;
	}

	return 0;


	
}




/**
* @brief Saves the time data given in a struct.
*
* @param newtime A pointer to a struct with the new time.
*
* @return void
*/
void DS3231M_set_time(struct tm *newtime)
{

	//Funtrace_enter(12);

	if (!connected.TWI)	{return;}
	
	//FUNCTION_TRACE


	if (!check_valid_ranges(newtime)){return;}
	


	uint8_t data[7];
	


	data[0] = ((newtime->tm_sec / 10) << 4)  | (newtime->tm_sec % 10);
	data[1] = ((newtime->tm_min / 10) << 4)  | (newtime->tm_min % 10);
	data[2] = ((newtime->tm_hour / 10) << 4)  | (newtime->tm_hour % 10);
	data[3] = 1;
	data[4] = ((newtime->tm_mday / 10) << 4)  | (newtime->tm_mday % 10);
	data[5] = ((newtime->tm_mon  / 10) << 4)  | (newtime->tm_mon % 10);
	data[6] = ((newtime->tm_year / 10) << 4)  | (newtime->tm_year % 10);
	
	

	//newtime->tm_year += 100;
	
	
	struct tm Time_copy;
	
	Time_copy.tm_sec = newtime->tm_sec;
	Time_copy.tm_min = newtime->tm_min;
	Time_copy.tm_hour = newtime->tm_hour;
	Time_copy.tm_mday = newtime->tm_mday;
	Time_copy.tm_mon = newtime->tm_mon - 1 ;
	Time_copy.tm_year = newtime->tm_year +100;
	Time.tm_isdst =0;
	
	
	
	
	
	last_Valid_Time = mktime(&Time_copy);

	
	uint8_t i2c_ret_code = I2C_write_to(DS3231M_address,DS3231M_address_seconds, data, 7);
	
	if(i2c_ret_code == I2C_ERROR ){
		print_info_DS3231M("i2cERR",0);
		connected.TWI = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		_delay_ms(2000);
		return;
	}
	if(i2c_ret_code == DEVICE_NOT_CONNECTED){
		connected.DS3231M = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		return;
	}
}



/**
* @brief Reads the current time and saves the values in the time variables
*
*
* @return void
*/
void DS3231M_read_time(void)
{
	//Funtrace_enter(13);
	if (!connected.TWI)	{return;}
	
	uint8_t data[7];
	uint8_t i2c_ret_code = I2C_read_from(DS3231M_address,DS3231M_address_seconds,data,7);
	
	if(i2c_ret_code == I2C_ERROR ){
		print_info_DS3231M("i2cERR",0);
		connected.TWI = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		_delay_ms(2000);
		return;
	}
	if(i2c_ret_code == DEVICE_NOT_CONNECTED){
		connected.DS3231M = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		return;
	}

	uint8_t sec = data[0];
	uint8_t min = data[1];
	uint8_t hour = data[2];
	uint8_t mday = data[4];
	uint8_t mon = data[5];
	uint8_t year = data[6];
	
	if ((sec == 0) && (min == 0) && (hour == 0) && (mday == 0) && (mon == 0) && (year == 0))
	{
		connected.DS3231M = 0;
		SET_ERROR(TIMER_ERROR);
		return;
		}else{
		connected.DS3231M = 1;
		connected.TWI =1;
		
		CLEAR_ERROR(TIMER_ERROR);
	}
	
	//  Bits 7-4 are for the tens digits and bits 0-3 are for unit digits
	
	//			   | 3210 |		 | 7654|
	Time.tm_sec  = (sec & 0x0F) + (sec >> 4)*10;
	
	Time.tm_min  = (min & 0x0F) + (min >> 4)*10;

	Time.tm_hour = (hour & 0x0F) + (hour >> 4)*10;

	Time.tm_mday = (mday & 0x0F) + (mday>> 4)*10;

	Time.tm_mon  = (mon & 0x0F) + (mon >> 4)*10;

	Time.tm_year = (year & 0x0F) + (year >> 4)*10 ;
	

	

	
	
}

void DS3231M_read_temperature(void)
{
	uint8_t data[2];

	uint8_t i2c_ret_code = I2C_read_from(DS3231M_address,DS3231M_address_temp_MSB,data,2);
	
	if(i2c_ret_code == I2C_ERROR ){
		print_info_DS3231M("i2cERR",0);
		connected.TWI = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		_delay_ms(2000);
		return;
	}
	if(i2c_ret_code == DEVICE_NOT_CONNECTED){
		connected.DS3231M = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		return;
	}

	
	temp_MSB=data[0];
	temp_LSB=data[1];

	
	DS3231M_Temperature = ((float) temp_MSB + ((float) (temp_LSB >> 6) / 4) + 273);
}


