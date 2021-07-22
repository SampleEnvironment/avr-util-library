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

extern DS3231MType DS_state;

uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t lenght);
uint8_t I2C_write_to(uint8_t I2C_Bus_address, uint8_t register_address, uint8_t *data,uint8_t length);

#endif