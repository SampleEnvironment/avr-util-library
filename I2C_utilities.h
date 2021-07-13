/*
*
 * IncFile1.h
 *
 * Created: 26.10.2020 13:07:23
 *  Author: Peter Wegmann
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_


#include <util/delay.h>
#include "i2cmaster.h"


  #define LOW 0
  
  #define INPUT &= ~
  
  #define  OUTPUT |=
  
  #define SDA PINC1
  #define SCL PINC0
  
  #define pinMode(X,Y) (DDRC Y (_BV(X)))
  
  #define  INPUT_PULLUP(X) DDRC &= ~ (_BV(X)); PORTC |= _BV(X);
  #define  INPUT_NO_PULLUP(X) DDRC &= ~ (_BV(X)); PORTC &= ~(_BV(X));
  
  
  #define digitalRead(X) ((PORTC & (1<<X)) && 1)
  
  #define false 0
  #define true 1
  
  // I²C Codes

  #define DEVICE_NOT_CONNECTED 2
  #define I2C_ERROR 1
  #define I2C_SUCCESS 0
  
  /**
  * @brief Connection Status of BMP and DS3231M
  *
  * Holds the Status of the connection to the BMP sensor (Pressure and Temperature Sensor) and the DS3231M (Time and Date) Chip.
  */
  typedef struct{
	  uint8_t BMP;            /**< @brief BMP Connection */
	  uint8_t BMP_on_Startup; /** @brief BMP connected on Startup */
	  uint8_t DS3231M;		  /**< @brief DS3231M Connection */
	  uint8_t TWI;			  /**< @brief  Indication of I2C-Bus connectivity for preventing loops  */
  }connectedType;

extern connectedType connected;

uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t lenght);
uint8_t I2C_write_to(uint8_t I2C_Bus_address, uint8_t register_address, uint8_t *data,uint8_t length);
int I2C_ClearBus(void);

#endif /* INCFILE1_H_ */