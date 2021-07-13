/*
* I2C_utilities.c
*
* Created: 26.10.2020 13:07:08
*  Author: Weges
*/

#include "I2C_utilities.h"
#include "i2cmaster.h"



/**
* @brief Connection Status of BMP and DS3231M
*
* Holds the Status of the connection to the BMP sensor (Pressure and Temperature Sensor) and the DS3231M (Time and Date) Chip.
*/
connectedType connected = {.BMP = 0,.DS3231M=0,.TWI = 1};




uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t lenght)
{
	
	if (i2c_start(I2C_Bus_address+I2C_WRITE)!=0)
	{
		i2c_stop();
		return DEVICE_NOT_CONNECTED;
	}
	
	i2c_write(register_address);
	
	
	if(i2c_rep_start(I2C_Bus_address+I2C_READ) != 0)
	{
		i2c_stop();
		return I2C_ERROR;
	}
	
	for (uint8_t i = 0 ; i < lenght-1 ; i++ )
	{
		data[i] = i2c_readAck();
	}
	
	data[lenght-1] = i2c_readNak();
	

	i2c_stop();
	return I2C_SUCCESS;
}

uint8_t I2C_write_to(uint8_t I2C_Bus_address, uint8_t register_address, uint8_t *data,uint8_t length)
{
	if (i2c_start(I2C_Bus_address+I2C_WRITE)!=0)
	{
		i2c_stop();
		return DEVICE_NOT_CONNECTED;
		
	}
	if (i2c_write(register_address))
	{
		i2c_stop();
		return I2C_ERROR;
	}
	
	
	for (uint8_t i = 0 ; i< length; i++)
	{
		
		if (i2c_write(data[i]))
		{
			i2c_stop();
			return I2C_ERROR;
		}
		
	}
	
	i2c_stop();
	return I2C_SUCCESS ;

}



int I2C_ClearBus(void) {




	

	INPUT_PULLUP(SDA);; // Make SDA (data) and SCL (clock) pins Inputs with pullup.
	INPUT_PULLUP(SCL);

	_Bool SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
	if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master.
		return 1; //I2C bus error. Could not clear SCL clock line held low
	}

	_Bool SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
	int clockCount = 20; // > 2x9 clock

	while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
		clockCount--;
		// Note: I2C bus is open collector so do NOT drive SCL or SDA high.
		INPUT_NO_PULLUP(SCL); // release SCL pullup so that when made output it will be LOW
		pinMode(SCL, OUTPUT); // then clock SCL Low
		_delay_us(10); //  for >5uS
		pinMode(SCL, INPUT); // release SCL LOW
		INPUT_PULLUP(SCL); // turn on pullup resistors again
		// do not force high as slave may be holding it low for clock stretching.
		_delay_us(10); //  for >5uS
		// The >5uS is so that even the slowest I2C devices are handled.
		SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
		int counter = 20;
		while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
			counter--;
			_delay_ms(100);
			SCL_LOW = (digitalRead(SCL) == LOW);
		}
		if (SCL_LOW) { // still low after 2 sec error
			return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
		}
		SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
	}
	if (SDA_LOW) { // still low
		return 3; // I2C bus error. Could not clear. SDA data line held low
	}

	// else pull SDA line low for Start or Repeated Start
	INPUT_NO_PULLUP(SDA); // remove pullup.
	pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
	// When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
	/// A Repeat Start is a Start occurring after a Start with no intervening Stop.
	_delay_us(10); // wait >5uS
	pinMode(SDA, INPUT); // remove output low
	INPUT_PULLUP(SDA);; // and make SDA high i.e. send I2C STOP control.
	_delay_us(10); // x. wait >5uS
	
	i2c_init();

	return 0; // all ok
}