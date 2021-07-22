
#include "Mock_I2C_utilities.h"
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "../DS3231M.h"

#define assertm(exp, msg) assert(((void)msg, exp))

DS3231MType DS_state;



/**
* @brief Connection Status of BMP and DS3231M
*
* Holds the Status of the connection to the BMP sensor (Pressure and Temperature Sensor) and the DS3231M (Time and Date) Chip.
*/
connectedType connected = {.BMP = 0,.DS3231M=0,.TWI = 1};




uint8_t I2C_read_from(uint8_t I2C_Bus_address ,uint8_t register_address, uint8_t * data,uint8_t length)
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
        switch (register_address)
        {
        case DS3231M_address_temp_MSB:
            memcpy(data,DS_state.temp,length);
            break;
        case DS3231M_address_control_reg:
            data[0] = DS_state.controlReg ;
            assertm(length == 1,"lenght must equal 1 for reading from control_reg" );
            break;
        case DS3231M_address_seconds:
            memcpy(data,DS_state.date,length);
        break;
        
        default:
            break;
        }

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
        switch (register_address)
        {
        case DS3231M_address_temp_MSB:
            memcpy(DS_state.temp,data,length);
            break;
        case DS3231M_address_control_reg:
            DS_state.controlReg = data[0] ;
            assertm(length == 1,"lenght must equal 1 for writing to control_reg" );
            break;
        case DS3231M_address_seconds:
            memcpy(DS_state.date,data,length);
            break;
        }

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