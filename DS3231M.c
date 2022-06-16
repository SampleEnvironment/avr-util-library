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
#include <stdlib.h>
#include <util/eu_dst.h>


#include "DS3231M.h"
#include "status.h"


#ifndef UNIT_TEST
#include "I2C_utilities.h"
#include <util/eu_dst.h>
#else
#include <stdio.h>
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
time_t sys_Time;


int8_t temp_MSB;
uint8_t temp_LSB;
double DS3231M_Temperature;


/**
* @brief Holds Function to print Infoline
*
* Paints an info message on the LCD
*/
void (*print_info_DS3231M)(char * ,bool ) = NULL;





/**
* @brief  Initialization of the external device DS3231M, a time holding unit. It's connected via I2C (Two Wire Interface).
*
*
* @return uint8_t  0 on success and greater zero if initialization was unsuccessful
*
*/
uint8_t init_DS3231M(void (*printInfoFun)(char *,bool))
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
	Time_copy.tm_isdst = -1;
	


	
	set_dst(&eu_dst);
	set_zone(0);

	set_system_time(mktime(&Time_copy));
	
	if (!connected.TWI)	{return;}
	
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
	if (!connected.TWI)	{

		DS3231M_estimate_sys_Time();

		return;
	}
	
	uint8_t data[7];
	uint8_t i2c_ret_code = I2C_read_from(DS3231M_address,DS3231M_address_seconds,data,7);
	
	if(i2c_ret_code == I2C_ERROR ){
		print_info_DS3231M("i2cERR",0);
		connected.TWI = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);
		_delay_ms(2000);


		DS3231M_estimate_sys_Time();

		return;
	}
	if(i2c_ret_code == DEVICE_NOT_CONNECTED){
		connected.DS3231M = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TIMER_ERROR);


		DS3231M_estimate_sys_Time();

		return;
	}

	struct tm ds_Time;

	ds_Time.tm_sec  = decodeDS3231M(data[0]);
	ds_Time.tm_min  = decodeDS3231M(data[1]);
	ds_Time.tm_hour = decodeDS3231M(data[2]);
	ds_Time.tm_mday = decodeDS3231M(data[4]);
	ds_Time.tm_mon  = decodeDS3231M(data[5]);
	ds_Time.tm_year = decodeDS3231M(data[6]);
	
	if (DS3231M_concurrency_check(&ds_Time))
	{
		connected.DS3231M = 1;
		connected.TWI =1;
		
		CLEAR_ERROR(TIMER_ERROR);


		memcpy(&Time,&ds_Time,sizeof(struct tm));


		}else{
		SET_ERROR(TIMER_ERROR);
		

		DS3231M_estimate_sys_Time();


		return;

	}
	



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


uint8_t encodeDS3231M(uint8_t element){
	return ((element / 10) << 4)  | (element % 10);
}

uint8_t decodeDS3231M(uint8_t element){
	return (element & 0x0F) + (element >> 4)*10;
}

void DS3231M_estimate_sys_Time(void){
	
	time(&sys_Time);
	

	struct tm * curr_t_est  = 	localtime(&sys_Time);

	Time.tm_sec  = curr_t_est->tm_sec;
	Time.tm_min  = curr_t_est->tm_min;
	Time.tm_hour = curr_t_est->tm_hour;
	Time.tm_mday = curr_t_est->tm_mday;
	Time.tm_mon  = curr_t_est->tm_mon  + 1;   // tm_mon (0...11) but in ds3231m --> 1..12
	Time.tm_year = curr_t_est->tm_year - 100; // tm_year (1900...) but in ds3231m --> 2000...
}

uint8_t DS3231M_concurrency_check(  struct tm *ds_Time){
	if (
	!(INRANGE(ds_Time->tm_sec,0,59) &&
	INRANGE(ds_Time->tm_min,0,59) &&
	INRANGE(ds_Time->tm_hour,0,23) &&
	INRANGE(ds_Time->tm_mday,1,31) &&
	INRANGE(ds_Time->tm_mon,1,12) &&
	INRANGE(ds_Time->tm_year,21,99))
	)
	{
		return 0;
	}

	return 1;



}