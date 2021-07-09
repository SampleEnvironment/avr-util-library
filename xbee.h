// xbee.h - Copyright 2016, HZB, ILL/SANE & ISIS

#ifndef XBEE_H_
#define XBEE_H_

#include <util/delay.h>
#include "main.h"


//====================================================================
//			XBEE
//====================================================================
typedef struct
{
	uint32_t 	dest_low ;	//0x1234;
	uint32_t 	dest_high ;	//0x5678;

	// Variables for XBee interface
	uint8_t		sleep_period;		// in minutes
	uint8_t		awake_period;     // in seconds
	_Bool 		sleeping ;
	uint8_t     status_byte;				// Status byte
	
}XbeeType;

//==============================================================
// Device status
//==============================================================

enum StatusBit
{
	NETWORK_ERROR,
	NO_REPLY_ERROR,
	STARTED_FILLING_ERROR,
	STOPPED_FILLING_ERROR,
	CHANGED_OPTIONS_ERROR,
	SLOW_TRANSMISSION_ERROR,
	LETTERS_ERROR,
};

//TODO RF Data can only be 100 Bytes --> TEST CASE
#define SINGLE_FRAME_LENGTH 	256		// Full length of one frame  ATTENTION if changed then change DATA_LENGTH in xbee_utilities.h as well


//#define XBEE_AWAKE_TIME			30		// Time in seconds

extern volatile uint8_t *send_str_reader;			// Pointer to the next byte to send via USART
extern volatile uint8_t sending_cmd;				// Number of bytes to send via USART

//==============================================================
// Database server commands
//==============================================================
// Requests with answer sent from the device
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
#define GET_PULSE_I				25		// Send I Values of Pluse
#define GET_PULSE_U				26		// Send U Values of Pluse

// Requests sent from the database server
#define SET_OPTIONS_CMD			13		// Set device settings received from the database server
#define SET_LETTERS_CMD			15		// Set list of available device positions received from the database server
#define SET_PASSWORD_CMD		17		// Set options password received from the database server. This password is required to access to the settings pages.
#define SET_XBEE_SLEEP_TIME_CMD	19		// Set XBee sleeping period received from the database server
#define DISPLAY_MSG_CMD	        20		// Displays a message on the display
#define SET_XBEE_AWAKE_TIME_CMD	22		// Set XBee awake period received from the database server
#define TRIGGER_MEAS_MSG	    24      // Triggers a measurement of the Helium level
#define TRIGGER_REMOTE_PULSE    27		// Trigger a remote Pulse
#define TRIGGER_REMOTE_U_OVER_I 28		// Trigger a remote U over I Pulse

#define ILM_BROADCAST_MSG       99      // ILM messages are sent in broadcast mode and are ignored by other devices



extern volatile uint8_t Xbee_Associated;





//==============================================================
// XBee commands
//==============================================================

void xbee_set_busy(_Bool busy_state);
_Bool xbee_get_busy(void);
void xbee_set_sleeping(_Bool sleeping);
_Bool xbee_get_sleeping(void);
void xbee_set_awake_period(uint8_t awake_period);
uint8_t xbee_get_awake_period(void);
void xbee_set_sleep_period(uint8_t sleep_period);
uint8_t xbee_get_sleep_period(void);
void xbee_set_status_byte(uint8_t status_byte);
uint8_t xbee_get_status_byte(void);


void xbee_sleep(void);			// Set XBee module to sleep mode
void xbee_wake_up(void);			// Wake up XBee module
void xbee_wake_up_plus(void);
void xbee_sleep_plus(void);

uint8_t xbee_is_connected(void);           // checks if xbee is connected to coordinator
_Bool xbee_reset_connection(void);		// Reset connection with the xbee coordinator and initiate a new one
// Returns true if reconnection is successfull, false otherwise
uint16_t xbee_hardware_version(void);
uint8_t xbee_reconnect(void);			// Try a new connection with the server
void xbee_send(uint8_t *data);			// Start USART0 transmission to XBee module
void xbee_send_msg(uint8_t *buffer, uint8_t length);	// Send message via XBee module
uint8_t xbee_send_and_get_reply(uint8_t *buffer, uint8_t length, uint8_t db_cmd_type, uint16_t delay);	// Send message (if length not 0) and look for reply from database and copy data to the buffer
uint8_t xbee_send_request_only(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);
uint8_t xbee_send_request(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);	// Checks network connection and send request and receive answer
uint8_t xbee_send_message(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length);      // Checks network connection and sends message
// moved to main uint8_t xbee_send_login_msg(uint8_t db_cmd_type, uint16_t device_id, uint8_t *buffer, uint32_t dest_high, uint32_t dest_low);	// Login transaction

_Bool xbee_get_server_adrr(void);

void SET_ERROR(enum StatusBit Bit);
void CLEAR_ERROR(enum StatusBit Bit);
_Bool CHECK_ERROR(enum StatusBit Bit);


void delay_ms(uint16_t period);	 // User defined delay routine (milliseconds)

#endif /* XBEE_H_ */
