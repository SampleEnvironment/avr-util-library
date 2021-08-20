#ifndef MOCK_AMIN
#define MOCK_MAIN




#include <stdint.h>


#define MEASBUFFER_LENGTH      250      /**< @brief Maximum number of concurrently saved Measurements*/
#define MEASUREMENT_MESSAGE_LENGTH 23  /**< @brief Number of Bytes that makes up one Measurement Message*/
#define NUMBER_LOGIN_BYTES	    39     /**< @brief Number of Bytes in login Message from Server   */

#define COM_TIMEOUT_TIME 		10	/**< @brief Communication Timeout-period in Seconds  */






#define USE_DISPLAY
#define ALLOW_COM					// Allow the possibility to receive answer from XBee module
#define ALLOW_DEBUG
#define ALLOW_LOGIN
#define ENABLE_ASSERTIONS
//#define ENABLE_FUNC_TRACE
//#define DEBUG_OPTIONS
//#define DEBUG_DS3231M
//#define TP_comp_debug
//#define force_tp_comp


#define default_offsetValue						0		// in 1 mL
#define default_offsetVolume					0		// in 1 mL
#define default_offsetCorrVolume				0		// in 1 mL
#define default_Value							0		// in 1 mL
#define default_Volume							0		// in 1 mL
#define default_CorrVolume						0		// in 1 mL
#define default_t_transmission_min				5		// in seconds
#define default_t_transmission_max				120		// in minutes
#define default_delta_V							100000	// in 1 mL
#define default_delta_p							100		// in 0.1 mbar
#define default_step_Volume						10000	// in 1 mL // normally 10L
#define default_offset_pressure					0		// in 0.1 mbar
#define default_span_pressure					10000	// in 10^-4
#define default_T_Compensation_enable			0		// 1...ON	0...Off
#define default_p_Compensation_enable			0		// 1...ON	0...Off
#define default_Temperature_norm				2882	// in 0.1K //15.0 C as default reference
#define default_Pressure_norm					10133	// in 0.1 mBar


#define MAX_NOMINAL_PRESSURE			117000 // 1170.00 mbar
#define MIN_NOMINAL_PRESSURE             95000 // 950.00 mbar
#define MAX_NOMINAL_TEMPERATURE			  3280  // 55�C
#define MIN_NOMINAL_TEMPERATURE			  2631  //-10�C

#define MAX_PRESSURE_DELTA			   12000 //  120mbar/5s		
#define MAX_TEMPERATURE_DELTA			 130 //  13�C/5s

#define FUNTRACE_ARRAY_SIZE			10
#define FUNTRACE_HEADER_LEN			7
#define FUNTRACE_PASS				43690





#define SET_NETWORK_ERROR 				status.device|=(1<<0);ex_mode = ex_mode_offline;
#define SET_NO_REPLY_ERROR 				status.device|=(1<<1);
#define SET_OPTIONS_ERROR 				status.device|=(1<<2);status.device_reset_on_Send |= status.device;
#define SET_TEMPPRESS_ERROR 			status.device|=(1<<3);status.device_reset_on_Send |= status.device;
#define SET_TIMER_ERROR					status.device|=(1<<4);status.device_reset_on_Send |= status.device;
#define SET_INIT_OFFLINE_ERROR          status.device|=(1<<5);
#define SET_I2C_BUS_ERROR				status.device|=(1<<6);status.device_reset_on_Send |= status.device;
#define SET_VOLUME_TOO_BIG_ERROR		status.device|=(1<<7);status.device_reset_on_Send |= status.device;

#define CLEAR_NETWORK_ERROR 			status.device&=~(1<<0);ex_mode = ex_mode_online;
#define CLEAR_NO_REPLY_ERROR 			status.device&=~(1<<1);
#define CLEAR_OPTIONS_ERROR 			status.device&=~(1<<2);
#define CLEAR_TEMPPRESS_ERROR 			status.device&=~(1<<3);
#define CLEAR_TIMER_ERROR			    status.device&=~(1<<4);
#define CLEAR_INIT_OFFLINE_ERROR        status.device&=~(1<<5);
#define CLEAR_I2C_BUS_ERROR             status.device&=~(1<<6);
#define CLEAR_VOLUME_TOO_BIG_ERROR		status.device&=~(1<<7);

#define CHECK_NETWORK_ERROR 			(status.device & (1<<0))
#define CHECK_NO_REPLY_ERROR 			(status.device & (1<<1))
#define CHECK_OPTIONS_ERROR 			(status.device & (1<<2))
#define CHECK_TEMPPRESS_ERROR 			(status.device & (1<<3))
#define CHECK_TIMER_ERROR				(status.device & (1<<4))
#define CHECK_INIT_OFFLINE_ERROR        (status.device & (1<<5))
#define CHECK_I2C_BUS_ERROR				(status.device & (1<<6))
#define CHECK_VOLUME_TOO_BIG_ERROR      (status.device & (1<<7))


/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
//#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1<<(b))))






/**
* @brief Time Pressure or Temperature deltas
*
* All Variables in Gascounter representing a difference in Time, Pressure or Temperature since the last Measuerement/Send/Reset
*/
typedef struct {
	uint32_t Volume_since_last_send;       /**< @brief Volume   since last send/save: resets to 0 every time the data is sent or saved in 1 mL */
	uint16_t Pressure_since_last_send;     /**< @brief Pressure change since last send/save: resets to 0 every time the data is sent or saved (in 0.1 mbar) */
	uint32_t t_send;					   /**< @brief Time since last sending of Data */
	uint32_t t_Display_reset;			   /**< @brief Time since last Display Reset */
} deltaType;


/**
* @brief Pressure- & Timestamps of previous Events
*
* Absolute Times/Pressure of the last time an action was executetd i.e: Pinging the Server
*/
typedef struct {  
	uint32_t Pressure_on_send;         /**< @brief Pressure the last time Data was sent/stored to the Server/memory (in 0.1 mbar)*/
	uint32_t time_send;				   /**< @brief Absolute time of last sending of Data*/
	uint32_t time_display_reset;       /**< @brief Absolute time of last Display Reset*/
	uint32_t time_ping;				   /**< @brief Absolute time of last Ping*/
	uint32_t time_Pressure_Temp_meas;  /**< @brief Absolute time of last Temp and Pressure Measurement */
	uint32_t time_TWI_action;          /**< @brief Absolute time of last TWI read/write/stop */
	uint32_t time_I2C_check;		   /**< @brief Absolute time of last I2C Peripheral checkup */
	uint32_t time_valid_time_reading;   /**< @brief Absolute time of last Valid Time reading  */
}lastType;




/**
* @brief Options of the Gascounter
*
* Struct containing fields for all options of a Gascounter
*/
typedef struct{
	uint64_t offsetValue;			/**< @brief Offset for uncorrected Volume from Database (in 1ml)*/
	uint64_t offsetVolume;			/**< @brief Offset for uncorrected Volume from Database (in 1ml)*/
	uint64_t offsetCorrVolume;		/**< @brief Temperature and Pressure offset of corrected Volume of Gascounter. Should correspond to the Value on the analog Gascounter when receiving Options on startup (in 1ml) */
	uint64_t Value;				    /**< @brief Uncorrected Volume, periodically sent to Server (in 1ml)*/
	uint64_t Volume;				/**< @brief Uncorrected Volume, periodically sent to Server (in 1ml)*/
	uint64_t CorrVolume;			/**< @brief Corrected Volume, periodically sent to Server (in 1ml)*/
	uint16_t t_transmission_min;	/**< @brief Minimal time between Measurement-Data Transmissions (in seconds) */
	uint16_t t_transmission_max;	/**< @brief Maximum time between Measurement-Data Transmissions (in minutes) */
	uint32_t delta_V;				/**< @brief Delta Volume since last Measurement-Data Transmission (in 1ml) */
	uint16_t delta_p;				/**< @brief Delta Pressure since last Measurement-Data Transmission (in 0.1mbar) */
	uint32_t step_Volume;			/**< @brief Volume-Step that corresponds to one Tick from the Gascounter. Specific for the kind of Gascounter that is used. (in 1ml) */
	int16_t offset_pressure;		/**< @brief Offset Pressure for Altitude compensation (in 0.1mbar) */
	uint16_t span_pressure;			/**< @brief Span Pressure ?? in 10^-4 */
	uint8_t T_Compensation_enable;  /**< @brief Temperature-Compensation Boolean 1 = ON; 0 = OFF */
	uint16_t Temperature_norm;      /**< @brief Normal Temperature usually 15C (in 0.1K) */
	uint8_t p_Compensation_enable;  /**< @brief Pressure-Compensation Boolean 1 = ON; 0 = OFF */
	uint16_t Pressure_norm;			/**< @brief Normal Pressure usually 1033.3mbar (in 0.1mbar) */
}optType;



/**
* @brief USART communication
*
* Global Variables/Pointers needed for USART Communication
*/
typedef struct{
	uint8_t cmd_line;			/**< @brief USART Receive Block (Data is received bytewise)*/
	uint8_t *send_str_reader;	/**< @brief Pointer to the next byte to send via USART*/
	uint8_t sending_cmd;		/**< @brief Number of bytes to send via USART */
}usartType;



/**
* @brief Old Volume Measurement
*
* Holds the old #Value, #Volume and #CorrVolume Measurement in order to compare them with the new one  to check for overflows
*/
typedef struct{
	uint64_t Value ;		/**< @brief  Holds the old options#Value to compare them with the new to check for overflows.  */
	uint64_t Volume;		/**< @brief  Holds the old options#Volume to compare them with the new to check for overflows.  */
	uint64_t CorrVolume ;	/**< @brief  Holds the old options#CorrVolume to compare them with the new to check for overflows   */
}oldType;




/**
* @brief Status related information
*
*
* Contains Status-bytes for internal and external Status processing and reporting
*/
typedef struct
{
	uint8_t byte_90; /**< @brief  sent on registration with Server (#CMD_send_registration_90)*/
	//uint8_t byte_91; /**< @brief  sent on Measurement Data Transmission  (#CMD_send_data_91). communicates Device Status to server.*/
	uint8_t byte_92; /**< @brief  sent in response to send options Command (#CMD_send_options_92). Is currently always set to zero.   */
	//uint8_t byte_93; /**< @brief  sent in response to receiving options (#CMD_send_response_options_set_93). The 0th Bit set to one indicates an error with setting Options */
	//uint8_t byte_94;    always equals byte_91
	uint8_t byte_95; /**< @brief sent with every Ping (#CMD_send_Ping_95). Currently always set to zero. */
	uint8_t byte_96; /**< @brief  received with Options from the Server. Indicates wether Volume-offsets should be kept or updated */
	uint8_t device;  /**< @brief  Status-Byte for internal processing and handling execution states.More detailed explanation in...*/
	uint8_t device_reset_on_Send;
}statusType;




extern volatile uint32_t count_t_elapsed;
extern volatile statusType status;

void _delay_ms(uint16_t delay);

#endif