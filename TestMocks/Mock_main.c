
#include "Mock_main.h"
/************************************************************************/
/* STATUS                                                               */
/************************************************************************/

/**
* @brief Connection Status of BMP and DS3231M
*
* Holds the Status of the connection to the BMP sensor (Pressure and Temperature Sensor) and the DS3231M (Time and Date) Chip.
*/
connectedType connected = {.BMP = 0,.DS3231M=0,.TWI = 1};

/**
* @brief Status related Information
*
*
* Contains Status-bytes for internal and external Status processing and reporting
*/
volatile statusType status={.byte_90=0,  .byte_92=0,  .byte_95=0, .byte_96=0, .device=0,.device_reset_on_Send = 0};


/************************************************************************/
/* USART                                                                */
/************************************************************************/
/**
* @brief USART communication
*
* Global Variables/Pointers needed for USART Communication
*/
volatile usartType USART= {.cmd_line=0 , .send_str_reader= 0 ,.sending_cmd = 0};


/************************************************************************/
/*  Deltas                                                              */
/************************************************************************/
/**
* @brief Dalta Values
*
* All Variables in the Gascounter representing a difference in Time, Pressure or Temperature since the last Measuerement/Send/Reset
*/
deltaType delta = {.Volume_since_last_send = 0,.Pressure_since_last_send = 0,.t_send = 0,.t_Display_reset = 0};


/************************************************************************/
/*  Time- and Pressurestamps                                            */
/************************************************************************/
/**
* @brief Time- and Pressurestamps
*
* Timestamps and Pressurevalues for last send/display-reset/ping - Event
*/
lastType last= {.Pressure_on_send = 0,.time_send = 0,.time_display_reset = 0,.time_ping = 0, .time_Pressure_Temp_meas = 0, .time_I2C_check = 0,.time_valid_time_reading = 0};



/************************************************************************/
/* Options                                                              */
/************************************************************************/
/**
* @brief Active Options
*
* Active Options that are Set in the Gascounter
*/
optType options = {.offsetValue = default_offsetValue,
	.offsetVolume = default_offsetVolume,
	.offsetCorrVolume = default_offsetCorrVolume,
	.Value = default_Value,
	.Volume = default_Volume,
	.CorrVolume = default_CorrVolume,
	.t_transmission_min = default_t_transmission_min,
	.t_transmission_max = default_t_transmission_max,
	.delta_V = default_delta_V,
	.delta_p = default_delta_p,
	.step_Volume = default_step_Volume,
	.offset_pressure = default_offset_pressure,
	.span_pressure = default_span_pressure,
	.T_Compensation_enable = default_T_Compensation_enable,
	.Temperature_norm = default_Temperature_norm,
	.p_Compensation_enable = default_p_Compensation_enable,
.Pressure_norm = default_Pressure_norm};



/**
* @brief Absoulte Time since Startup
*
* Time elapsed since Gascounter was turned on in s (Overflow occurs every 136 Years)
*/
volatile uint32_t count_t_elapsed = 0;


void _delay_ms(uint16_t delay){

}