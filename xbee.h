// xbee.h - Copyright 2016, HZB, ILL/SANE & ISIS

#ifndef XBEE_H_
#define XBEE_H_

#include <util/delay.h>
#include "module_globals.h"
#include "usart.h"
#include "../config.h"

typedef enum
{
	ONLINE,
	NO_SERVER,
	NO_NETWORK
	
}NETSTAT_TYPE;

//====================================================================
//			XBEE
//====================================================================
typedef struct
{
	uint32_t 	 dest_low ;	//0x1234;
	uint32_t 	 dest_high ;	//0x5678;

	// Variables for XBee interface
	uint8_t		 sleep_period;	// in minutes
	uint8_t		 awake_period;   // in seconds
	_Bool 		 sleeping ;
	volatile     uint8_t associated ;
	char *       dev_id_str;     // pointer to device ID
	uint8_t      dev_id_str_len; // max len of dev_id
	
	double       scanDurarion;
	const uint16_t     ScanChannels;
	uint16_t     ScanChannels_current;
	
	char         CoordIdentifier[21] ;
	NETSTAT_TYPE netstat;
	
	int8_t  RSSI; // rssi of last received packet
}XbeeType;





typedef struct {
	uint8_t IP_oct_1;
	uint8_t IP_oct_2;
	uint8_t IP_oct_3;
	uint8_t IP_oct_4;
}IP_v4Type;

//==============================================================
// Device status
//==============================================================


typedef enum
{
	XPORT,
	XBEE_V_S1 ,
	XBEE_V_SC2
	
}XBEE_HW_VERSION;



#define SC_MASK_DEFAULT 0x1FFE
#define SC_already_received_Pattern 0b10101010



#define COM_TIMEOUT_TIME 	5 // timeout for xbee connection in seconds


//TODO RF Data can only be 100 Bytes --> TEST CASE
#define SINGLE_FRAME_LENGTH 	256		// Full length of one frame  ATTENTION if changed then change DATA_LENGTH in xbee_utilities.h as well

//#define XBEE_AWAKE_TIME			30		// Time in seconds


extern UsartType USART0;
extern VersionType version;
extern XbeeType xbee;












//==============================================================
// Database server commands
//==============================================================
#define  DEPRECATED_ILM_BROADCAST 99

// Requests with answer sent from the device
#ifdef LEVELMETER

#define LOGIN_MSG				1		// Request for login
#define FORCE_LOGIN_MSG			2		// Force login ???
//#define LETTERS_REQUEST         23		// Request for the positions list (compare to command SET_LETTERS_CMD)  NOT USED AT THE MOMENT

// Requests with answer (or without answer if ALLOW_COM is false) sent from the device
#define LONG_INTERVAL_MSG 		3		// Message contains: Device ID / He, battery and pressure levels / Status   ATTENTION: in the case of failing communication with database server: store measurement for future transmission
#define OPTIONS_CHANGED_MSG		4		// Send device settings to the database
#define FILLING_BEGIN_MSG		5		// He filling started / Send device position
#define FILLING_MSG				6		// Send levels and status while filling										ATTENTION: in the case of failing communication with database server: store measurement for future transmission
#define FILLING_END_MSG			7		// He filling ended / Send levels and status								ATTENTION: in the case of failing communication with database server: store measurement for future transmission
#define LOGOUT_MSG				8		// Send levels and status just before shutting down
#define XBEE_ACTIVE_MSG			9		// Send device status and notify activity after a sleeping period
#define STATUS_MSG				10		// Send levels and status to the database server							ATTENTION: in the case of failing communication with database server: store measurement for future transmission




#define LAST_NON_CMD_MSG		10		// Search no ack commands only (see xbee_hasReply)





// Requests without answer sent from the device
#define UNKNOWN_MSG				11		// Unknown command received from database server
#define GET_OPTIONS_CMD			12		// Send device settings to the database server
#define GET_LETTERS_CMD			14		// Send list of available device positions to the database server
#define GET_PASSWORD_CMD		16		// Send options password to the database server. This password is required to access to the settings pages.
#define GET_XBEE_SLEEP_TIME_CMD	18		// Send XBee sleeping period to the database server
#define GET_XBEE_AWAKE_TIME_CMD	21		// Send XBee awake period to the database server
#define GET_PULSE_I				25		// Send I Values of Pulse
#define GET_PULSE_U				26		// Send U Values of Pulse

// Requests sent from the database server
#define SET_OPTIONS_CMD			13		// Set device settings received from the database server
#define SET_LETTERS_CMD			15		// Set list of available device positions received from the database server
#define SET_PASSWORD_CMD		17		// Set options password received from the database server. This password is required to access to the settings pages.
#define SET_XBEE_SLEEP_TIME_CMD	19		// Set XBee sleeping period received from the database server
#define DISPLAY_MSG_CMD	        20		// Displays a message on the display
#define SET_XBEE_AWAKE_TIME_CMD	22		// Set XBee awake period received from the database server
#define TRIGGER_MEAS_CMD	    24      // Triggers a measurement of the Helium level
#define TRIGGER_REMOTE_PULSE    27		// Trigger a remote Pulse
#define TRIGGER_REMOTE_U_OVER_I 28		// Trigger a remote U over I Pulse
#define TRIGGER_REMOTE_PULSE_CUSTOM_PARAMS_CMD 29 // Trigger a remote Pulse with custom Parameters
#define TRIGGER_REMOTE_PULSE_CONSTANT_I 30 // Trigger a remote Pulse with constant current I
#define SET_SC_XBEE_MASK        31      /**< @brief writes SC Channel Mask to device EEPROM */


//dummy Codes
#define PING_MSG			    0

#endif




#ifdef GASCOUNTER_MODULE
//==============================================================
// Database server commands GASCOUNTER_MODULE
//==============================================================
// Requests with answer sent from the device
#define LOGIN_MSG				90 /**< @brief Command for Registration with the Server. The server will answer a #CMD_received_set_options_96 message  */
#define MEAS_MSG						91 /**< @brief Command for sending Data to the Server. It is unanswered by the server */
#define PING_MSG						95 /**< @brief Command for Ping, the server will send a Pong with #CMD_received_Pong_89  */
//#define CMD_received_Pong_89					89 /**< @brief Command is received as an answer to a #CMD_send_Ping_95 message*/


// Requests without answer sent from the device
#define CMD_send_funtrace_88					88 /**< @brief Answering the funtrace request from the Server   */
//#define CMD_send_options_92						92 /**< @brief Command for sending current #options to the Server as an answer to #CMD_received_send_options_98  */
#define OPTIONS_SET_ACK		93 /**< @brief Sent to the server after options were received and set (#CMD_received_set_options_96) */
//#define CMD_send_response_send_data_94			94 /**< @brief Same as #CMD_send_data_91, but it is only sent as an answer to a #CMD_received_send_data_97 message   */


#define LAST_NON_CMD_MSG		95		// Search no ack commands only (see xbee_hasReply)



// Requests sent from the database server

#define SET_OPTIONS_CMD				    96 /**< @brief Received during the login process or during normal operation. It is used to send new options to the Gascounter module   */
#define TRIGGER_MEAS_CMD				97 /**< @brief Prompts device to send measurement data to the server */
#define GET_OPTIONS_CMD			        98 /**< @brief Prompts device to send current #options to the server */

#define SIMULATE_XBEE_CMD			   100
#define SET_FUNTRACE_CMD               101 /**< @brief Command for enabling or disabling the Functiontrace saving in eeprom */
#define GET_FUNTRACE_CMD			   102 /**< @brief Command prompting the device to send the function Trace that was saved in its eeprom*/
#define SET_PING_INTERVALL_CMD		   103 /**< @brief Command prompting the device to set the Ping Intervall*/
#define SET_IP_LAN_GASCOUNTER_CMD      104 /**< @brief Command prompting the device to set its IP address. The ip addres acts as identifier for the LAN-GCM */
#define SET_SC_XBEE_MASK               105 /**< @brief writes SC Channel Mask to device EEPROM */

//Dummy codes
#define FILLING_BEGIN_MSG   5
#define FILLING_END_MSG     7
#define OPTIONS_CHANGED_MSG 4
#define LONG_INTERVAL_MSG   3

#endif


#ifdef ILM_MODULE
//==============================================================
// Database server commands ILM_MODULE
//==============================================================
// Requests with answer sent from the device

#define PING_MSG			111     /**< @brief Command for Ping, the server will send a Pong with the same code  */
#define LOGIN_MSG			112		/**< @brief Command for Registration with the Server. The server will answer a 112C ommad containing device options*/
#define MEAS_MSG            113     /**< @brief ILM Measurement Message*/


// Requests without answer sent from the device
#define OPTIONS_SET_ACK		116 /**< @brief Sent to the server after options were received and set (#ILM_received_set_options) */

#define LAST_NON_CMD_MSG		120		// Search no ack commands only (see xbee_hasReply)

#define SET_OPTIONS_CMD		   121 /**< @brief Received during the login process or during normal operation. It is used to send new options to the Gascounter module   */
#define TRIGGER_MEAS_CMD	   122 /**< @brief Prompts device to send measurement data to the server */
#define GET_OPTIONS_CMD		   123 /**< @brief Prompts device to send current #options to the server */
#define SET_PING_INTERVALL_CMD 124 /**< @brief Command prompting the device to set the Ping Intervall*/
#define GET_RAW_DATA_CMD	   125 /**< @brief Command prompting the device to send raw ADC Values for all three channels */
#define SET_SC_XBEE_MASK       126 /**< @brief writes SC Channel Mask to device EEPROM */


//Dummy codes
#define FILLING_BEGIN_MSG   5
#define FILLING_END_MSG     7
#define OPTIONS_CHANGED_MSG 4
#define LONG_INTERVAL_MSG   3

#endif

extern void (*print_info_AT)(char *, _Bool );

//==============================================================
// XBee commands
//==============================================================

void xbee_init(void (*printInfoFun)(char *,_Bool),char * dev_ID_str,uint8_t max_devid_str_len,uint16_t SC_mask);

void xbee_set_busy(_Bool busy_state);
_Bool xbee_get_busy(void);
void xbee_set_sleeping(_Bool sleeping);
_Bool xbee_get_sleeping(void);
void xbee_set_awake_period(uint8_t awake_period);
uint8_t xbee_get_awake_period(void);
void xbee_set_sleep_period(uint8_t sleep_period);
uint8_t xbee_get_sleep_period(void);
char* xbee_get_coordID(void);



void xbee_sleep(void);			// Set XBee module to sleep mode
void xbee_wake_up(void);			// Wake up XBee module
void xbee_wake_up_plus(void);
void xbee_sleep_plus(void);


_Bool xbee_reset_connection(uint8_t force);		// Reset connection with the xbee coordinator and initiate a new one
// Returns true if reconnection is successfull, false otherwise


uint8_t xbee_reconnect(uint8_t force_DA);			// Try a new connection with the server
void xbee_send(uint8_t *data);			// Start USART0 transmission to XBee module
void xbee_send_msg(uint8_t *buffer, uint8_t length);	// Send message via XBee module
uint8_t xbee_send_and_get_reply(uint8_t *buffer, uint8_t length, uint8_t db_cmd_type, uint16_t comTimeOut);	// Send message (if length not 0) and look for reply from database and copy data to the buffer
uint8_t xbee_send_request_only(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);
uint8_t xbee_send_request(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);	// Checks network connection and send request and receive answer
uint8_t xbee_send_message(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);      // Checks network connection and sends message
// moved to main uint8_t xbee_send_login_msg(uint8_t db_cmd_type, uint16_t device_id, uint8_t *buffer, uint32_t dest_high, uint32_t dest_low);	// Login transaction

_Bool xbee_get_server_adrr(void);




void delay_ms(uint16_t period);	 // User defined delay routine (milliseconds)

#endif /* XBEE_H_ */
