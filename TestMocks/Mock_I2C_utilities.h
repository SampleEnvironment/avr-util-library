#ifndef MOCK_I2C_UTILITIES_
#define MOCK_I2C_UTILITIES_

#include <stdint.h>
#include <time.h>

#define DEVICE_NOT_CONNECTED 2
#define I2C_ERROR 1
#define I2C_SUCCESS 0

#define DS3231M_address	 0xD0
#define BMP_address      0xEE 

typedef struct{
    struct tm currT;
    uint8_t date[7];
    uint8_t temp[2];
    uint8_t controlReg;
    uint8_t connected;
    uint8_t error;

}DS3231MType;


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

extern DS3231MType DS_state;
extern connectedType connected;

uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t lenght);
uint8_t I2C_write_to(uint8_t I2C_Bus_address, uint8_t register_address, uint8_t *data,uint8_t length);

#endif