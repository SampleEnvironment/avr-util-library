/**
*	@file 		BMP085.h
*	@brief 		BMP085 Temerature and Pressure sensor header
*	@author		Klaus Kiefer, Peter Wegmann
*	@version	1.5
*	@date		2020/03/25
*
*
*/

#ifndef BMP085_H_
#define BMP085_H_



#include <avr/io.h>
#include <util/delay.h>



#define BMP_address 0xEE
#define BMP_register_address 0xF4
#define BMP_result_MSB_address 0xF6
#define BMP_result_LSB_address 0xF7
#define BMP_measure_temperature 0x2E
#define BMP_measure_pressure 0x34

extern volatile int16_t	BMP_Temperature;  /**< @brief  Temerature measured by the BMP in 0.1K   */
extern volatile uint32_t	BMP_Pressure;  /**< @brief Pressure measured by the BMP in mbar  */

#define  BMP_OSS 1   /**< @brief  chose oversampling setting : precision 0 = low power, 1 = standard, 2 = high, 3 ultra high  */


uint8_t init_BMP(void (*printInfoFun)(char *,_Bool));
int16_t BMP_get_value_from_address(uint8_t address);
uint8_t BMP_Connected(void);
int16_t BMP_calc_temperature(uint16_t UT);
int32_t BMP_calc_pressure(uint32_t UP);

uint16_t BMP_get_UT(void);
uint32_t BMP_get_UP(void);

uint8_t BMP_Temp_and_Pressure(void);
void BMP_I2C_ERROR_handling(uint8_t i2c_ret_code);

#endif 