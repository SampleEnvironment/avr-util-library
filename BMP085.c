/**
*	@file 		BMP085.c
*	@brief 		BMP085 Temerature and Pressure sensor API
*	@author		Klaus Kiefer, Peter Wegmann
*	@version	1.5
*	@date		2020/03/25
*
*	A detailed documentation of the Temperature- and Pressure-Sensor can be found in the <a href="BMP085 Datasheet.pdf" target="_blank"><b>BMP085 Datasheet</b></a>.
*
*/

#include <string.h>
#include <util/delay.h>

#include "BMP085.h"
#include "I2C_utilities.h"
#include "status.h"

#include "../config.h"

// specific constants varying from BMP to BMP
volatile int16_t AC1;
volatile int16_t AC2;
volatile int16_t AC3;
volatile uint16_t AC4;
volatile uint16_t AC5;
volatile uint16_t AC6;
volatile int16_t B1;
volatile int16_t B2;
volatile int16_t MB;
volatile int16_t MC;
volatile int16_t MD;
volatile int32_t B5;





volatile int16_t	BMP_Temperature = 0;  //**< @brief  Temerature measured by the BMP in 0.1K   */
volatile uint32_t	BMP_Pressure = 0;     //**< @brief Pressure measured by the BMP in mbar  */


volatile uint8_t BMP_press_MSB = 0;
volatile uint8_t BMP_press_LSB = 0;
volatile uint8_t BMP_press_XLSB = 0;
volatile uint8_t BMP_temp_MSB = 0;
volatile uint8_t BMP_temp_LSB = 0;



/**
* @brief Holds Function to print Infoline
*
* Paints an info message on the LCD
*/
void (*print_info_BMP)(char * infotext,_Bool update);


/**
* @brief Collects the device dependent values for several constants from the BMP085.
*
*
* @return uint8_t zero on success or a non-zero error value otherwise
*/
uint8_t init_BMP(void (*printInfoFun)(char *,_Bool))
{
	
	print_info_BMP = printInfoFun;
	
	if (!(AC1 = BMP_get_value_from_address(0xAA)))
	return 1;
	
	if (!(AC2 = BMP_get_value_from_address(0xAC)))
	return 1;
	
	if (!(AC3 = BMP_get_value_from_address(0xAE)))
	return 1;
	
	if (!(AC4 = (uint16_t)BMP_get_value_from_address(0xB0)))
	return 1;
	
	if (!(AC5 = (uint16_t)BMP_get_value_from_address(0xB2)))
	return 1;
	
	if (!(AC6 = (uint16_t)BMP_get_value_from_address(0xB4)))
	return 1;
	
	if (!(B1 = BMP_get_value_from_address(0xB6)))
	return 1;
	
	if (!(B2 = BMP_get_value_from_address(0xB8)))
	return 1;
	
	if (!(MB = BMP_get_value_from_address(0xBA)))
	return 1;
	
	if (!(MC = BMP_get_value_from_address(0xBC)))
	return 1;
	
	if (!(MD = BMP_get_value_from_address(0xBE)))
	return 1;
	
	return 0;
}


uint8_t BMP_Connected(void){
	if (!(AC1 = BMP_get_value_from_address(0xAA))){
		return 1;
	}
	connected.BMP_on_Startup = 1;
	return 0;
}



/**
* @brief Starts a I2C transmission with the BMP085 to read values from a certain register address.
*
* @param address A 8bit address of the register where to get the value from.
*
* @return int16_t A 16bit sign sensitive value which represents the data in the given register address.
*			If 0 is returned, there was an error on the I2C bus.
*/
int16_t BMP_get_value_from_address(uint8_t address)
{

	if (!connected.TWI)
	{
		return 0;
	}
	_delay_ms(200);

	uint8_t data[2];
	
	data[0] = address;
	
	
	uint8_t ret_code = I2C_read_from(BMP_address,address,data,2);
	
	if (ret_code)
	{
		BMP_I2C_ERROR_handling(ret_code);
		return 0;
	}
	
	
	
	return ((data[0] << 8) | data[1]);
}




/**
* @brief  Starts measurement of the Temperature and than gains the UT value for the calculation over a I2C transmission.
*
*
* @return uint16_t A 16bit number which represents the UT value.
*			If there is an error with the I2C connection, the function will return 0.
*/
uint16_t BMP_get_UT(void)
 {

	if(!connected.TWI){return 0;}
	uint8_t data[2];

	data[0] = BMP_measure_temperature;


	uint8_t ret_code =  I2C_write_to(BMP_address,BMP_register_address,data,1);
	if (ret_code)
	{
		BMP_I2C_ERROR_handling(ret_code);
		return 0;
	}

	_delay_ms(5); // wait for maeasurement to be done by BMP Sensor

	ret_code = I2C_read_from(BMP_address,BMP_result_MSB_address,data,2);

	if (ret_code)
	{
		BMP_I2C_ERROR_handling(ret_code);
		return 0;
	}

	return (data[0] << 8) | data[1];

}

/*
*	Function: BMP_get_UP
*
*	Description: Starts a measurement of the Pressure and than gains the UP value for the calculation over a I2C transmission.
*
*	Input:	/
*
*	Output:	A 32bit number which represents the UP value.
*			If there is an error with the I2C connection, the function will return 0.
*/


/**
* @brief Starts a measurement of the Pressure and than gains the UP value for the calculation over a I2C transmission.
*
*
*
* @return uint32_t 	A 32bit number which represents the UP value.
*			If there is an error with the I2C connection, the function will return 0.
*/
uint32_t BMP_get_UP(void)
{
	if(!connected.TWI){return 0;}
	uint8_t data[3];
	uint8_t amount_data_to_read;

	
	data[0] = BMP_measure_pressure + (BMP_OSS<<6);
	
	uint8_t ret_code = I2C_write_to(BMP_address,BMP_register_address,data,1);
	
	if (ret_code)
	{
		BMP_I2C_ERROR_handling(ret_code);
		return 0;
	}
	
	_delay_ms( 2 + (3 << BMP_OSS));
	
	if (BMP_OSS)
	{
		amount_data_to_read = 3;
	}
	else
	{
		amount_data_to_read = 2;
	}
	
	
	

	
	ret_code = I2C_read_from(BMP_address,BMP_result_MSB_address,data,amount_data_to_read);
	if (ret_code)
	{
		BMP_I2C_ERROR_handling(ret_code);
		return 0;
	}
	
	

	return ((uint32_t) data[0] << 16 | (uint32_t) data[1] << 8 | (uint32_t) data[2]) >> (8-BMP_OSS);
	
}



/**
* @brief  Calculates the temperature using the UT value. The unit of the value is 0.1K. For example: 2962 -> 296.2K
*
* @param UT Raw value supplied by the BMP chip
*
* @return int16_t A 16bit sign sensitive number which represents the temperature in 0.1K.
*/
int16_t BMP_calc_temperature(uint16_t UT)
{


	int16_t temperature;
	int32_t X1,X2;
	
	
	X1 = (((int32_t) UT - (int32_t) AC6) * (int32_t) AC5) >> 15;
	X2 = ((int32_t) MC << 11) / (X1 + MD);
	B5 = X1 + X2;

	temperature =  ((B5 + 8) >> 4) + 2732;


	return temperature;
}



/**
* @brief Calculates the pressure using the UP value. The unit of the value is 1Pa.
*
* @param UP Raw value supplied by the BMP chip
*
* @return int32_t  Pressure in Pa
*/
int32_t BMP_calc_pressure(uint32_t UP){

	
	int32_t pressure,X1,X2,X3,B3,B6;
	uint32_t B4, B7;
	
	B6 = B5 - 4000;
	//*****calculate B3************
	X1 = (B6*B6) >> 12;
	X1 *= B2;
	X1 >>= 11;
	
	X2 = (AC2*B6);
	X2 >>= 11;
	
	X3 = X1 +X2;
	
	B3 = (((((int32_t)AC1 )*4 + X3) <<BMP_OSS) + 2) >> 2;
	
	//*****calculate B4************
	X1 = (AC3* B6) >> 13;
	X2 = (B1 * ((B6*B6) >> 12) ) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (AC4 * (uint32_t) (X3 + 32768)) >> 15;
	
	B7 = ((uint32_t)(UP - B3) * (50000>>BMP_OSS));
	if( B7 < 0x80000000 )
	{
		pressure = (B7 << 1) / B4;
	}
	else
	{
		pressure = (B7 / B4) << 1;
	}
	
	X1 = pressure >> 8;
	X1 *= X1;
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * pressure  ) >> 16;
	pressure += (X1 + X2 + 3791) >> 4;        // pressure in Pa
	
	return pressure;

}



/**
* @brief Starts a Temperature and Pressure Measurement on the BMP and calculates the current Temperature and Pressure from the raw values (UP and UT). The results are written into the global variables #BMP_Pressure and # BMP_Temperature.
*
*
* @return uint8_t zero on success or a non-zero error value otherwise
*/
uint8_t BMP_Temp_and_Pressure(void){
	if (!connected.TWI)	{return 1;}

	
	uint32_t UP;
	uint16_t UT;
	int16_t temperature;
	int32_t pressure;
	
	
	UT = BMP_get_UT();
	if (UT == 0)
	{
		return 1;
	}
	UP = BMP_get_UP();
	if (UP == 0)
	{
		return 1;
	}
	if (!connected.TWI )
	{
		return 1;
	}
	temperature = BMP_calc_temperature(UT);
	pressure    = BMP_calc_pressure(UP);
	
	BMP_Pressure = (uint32_t) pressure;
	BMP_Temperature = temperature;
	
	return 0;
}


void BMP_I2C_ERROR_handling(uint8_t i2c_ret_code){
	if(i2c_ret_code == I2C_ERROR && connected.BMP_on_Startup ){
		print_info_BMP("i2cERR",0);
		connected.TWI = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TEMPPRESS_ERROR);
		_delay_ms(2000);
		return;
	}
	if(i2c_ret_code == DEVICE_NOT_CONNECTED && connected.BMP_on_Startup){
		connected.BMP = 0;
		SET_ERROR(I2C_BUS_ERROR);
		SET_ERROR(TEMPPRESS_ERROR);
		return;
	}
	if(i2c_ret_code == 0){
		connected.BMP_on_Startup = 1;
		return;
	}
}



















