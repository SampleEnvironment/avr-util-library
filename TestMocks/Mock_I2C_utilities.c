
#include "Mock_I2C_utilities.h"
#include <stdint.h>
#include <string.h>
#include <time.h>

DS3231MType DS_state;


uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t lenght)
{
	
    switch (I2C_Bus_address)
    {
    case DS3231M_address:
        if (!DS_state.connected)
        {
            return DEVICE_NOT_CONNECTED;
        }

        if (DS_state.error)
        {
            return I2C_ERROR;
        }
        
        memcpy(data,DS_state.date,lenght);
        return I2C_SUCCESS;
        
        break;

    case BMP_address:
        //TODO
        break;
    
    default:
        break;
    }

}

uint8_t I2C_write_to(uint8_t I2C_Bus_address, uint8_t register_address, uint8_t *data,uint8_t length)
{

      switch (I2C_Bus_address)
    {
    case DS3231M_address:
        if (!DS_state.connected)
        {
            return DEVICE_NOT_CONNECTED;
        }

        if (DS_state.error)
        {
            return I2C_ERROR;
        }
        
        memcpy(DS_state.date,data,length);
        return I2C_SUCCESS;
        
        break;

    case BMP_address:
        //TODO
        break;
    
    default:
        break;
    }

}

void setDS_state(DS3231MType state){
    memcpy(&DS_state,&state,sizeof(DS3231MType));

}