// xbee.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_XBEE 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on May 2016

//#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>



#include "usart.h"
#include "xbee_utilities.h"
#include "xbee.h"
#include "module_globals.h"
#include "printInfo_strings.h"
#include "status.h"





/**
* @brief Holds device specific Function to print Infoline
*
* Paints an info message on the LCD
*/
void (*print_info_xbee)(char *, _Bool )  = NULL;

//=========================================================================
// XBee variables
//=========================================================================
XbeeType xbee = {
	.dest_low = 0,
	.dest_high = 0,
	.sleep_period = 1,
	.awake_period = 120,
	.sleeping = false,
	.associated = 0
	
};

void xbee_init(void (*printInfoFun)(char *,_Bool),char * dev_ID_str,uint8_t max_devid_str_len){
	print_info_xbee = printInfoFun;
	xbee.dev_id_str = dev_ID_str;
	xbee.dev_id_str_len = max_devid_str_len;
}

void xbee_set_sleeping(_Bool sleeping){
	xbee.sleeping = sleeping;
}

_Bool xbee_get_sleeping(void){
	return xbee.sleeping;
}

void xbee_set_awake_period(uint8_t awake_period){
	xbee.awake_period = awake_period;
}

uint8_t xbee_get_awake_period(void){
	return xbee.awake_period;
}

void xbee_set_sleep_period(uint8_t sleep_period){
	xbee.sleep_period = sleep_period;
}

uint8_t xbee_get_sleep_period(void){
	return xbee.sleep_period;
}




// Set XBee module to sleep mode
inline void xbee_sleep(void)
{
	PORTA|=(1<<PA6);				// Set PA6 as true
	_delay_ms(20);

}

// Wake up XBee module
inline void xbee_wake_up(void)
{
	PORTA&=~(1<<PA6);				// Set PA6 as false
	_delay_ms(30);
	
}


uint8_t xbee_is_connected(void)
{
	uint8_t buffer[SINGLE_FRAME_LENGTH];
	uint8_t reply_Id = 0;

	
	// API command "AI" => Reads possible errors with the last association request, result 0: everything is o.k.
	buffer[0] = (uint8_t)'A';
	buffer[1] = (uint8_t)'I';
	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, 2);  	// Pack API "AI" command
	
	// Send packed command to the coordinator in order to check the connection
	reply_Id = xbee_send_and_get_reply(buffer, temp_bytes_number, AI_MSG_TYPE, 1000);

	#ifdef ALLOW_DEBUG
	print_info_xbee(XBEE_AI_MESSAGE, 0);
	_delay_ms(1000);
	#endif
	
	if (reply_Id== 0xFF)  	// NO Answer from xbee --> not associated
	{
		return 1;
	}
	if (frameBuffer[reply_Id].data[0] != 0)
	{
		// not associated with Coordinator
		return frameBuffer[reply_Id].data[0];
	}

	//Xbee is associated to a Coordinator
	return 0;

}


uint8_t xbee_hardware_version(void){
	#ifdef USE_XBEE

	uint8_t buffer[SINGLE_FRAME_LENGTH];
	
	buffer[0] = (uint8_t)'H';
	buffer[1] = (uint8_t)'V';
	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, 2);  	// Pack API "HV" command
	
	
	uint8_t reply_Id = xbee_send_and_get_reply(buffer, temp_bytes_number, HV_MSG_TYPE, 1000);

	if(reply_Id == 0xFF) return 0;
	
	
	uint16_t hw_version_16 = (frameBuffer[reply_Id].data[0] << 8) + frameBuffer[reply_Id].data[1] ;
	
	
	version.hw_version_xbee = (hw_version_16 > 0x2000)? XBEE_V_SC2 : XBEE_V_S1;
	#endif // USE_XBEE
	#ifdef USE_LAN
	version.hw_version_xbee = XPORT;
	#endif
	
	return version.hw_version_xbee;
}


// Reset connection with the xbee coordinator and initiate a new one
// Returns true if reconnection is successful, false otherwise
_Bool xbee_reset_connection(void)
{

	// Clear frame_buffer if there is still some information in the queue
	buffer_init();

	uint8_t buffer[SINGLE_FRAME_LENGTH];  		// put DA here then send
	
	// API command "DA" => Force Disassociation.
	// End Device will immediately disassociate from a Coordinator (if associated) and reattempt to associate.
	buffer[0] = (uint8_t)'D';
	buffer[1] = (uint8_t)'A';
	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, 2);  	// Pack API "DA" command
	
	// Send packed command to the Xbee-Module in order to Force Reassociation
	if (xbee_send_and_get_reply(buffer, temp_bytes_number, DA_MSG_TYPE, 1000) == 0xFF)
	return 0;											// couldn't disassociate (= no reply from xbee on my request)
	//dont need any data from DA so nothing else happens


	
	uint8_t timeout_count = 0;



	while(1){
		
		_delay_ms(100);
		print_info_xbee(XBEE_REASSOCIATE_  ,1);
		
		_delay_ms(100);
		print_info_xbee(XBEE_REASSOCIATE__ ,1);
		
		_delay_ms(100);
		print_info_xbee(XBEE_REASSOCIATE___,1);
		
		if((version.hw_version_xbee  == XBEE_V_S1)&& xbee.associated){
			print_info_xbee(XBEE_DEVICE_ASSOCIATED,1);
			return 1;
			
		}

		if(version.hw_version_xbee == XBEE_V_SC2 && xbee_is_connected() == 0){
			print_info_xbee(XBEE_DEVICE_ASSOCIATED,1);
			return 1;
		}
		
		
		if (timeout_count > 30)
		{
			print_info_xbee(XBEE_PRINT_NULL,0);
			print_info_xbee(XBEE_FAILED,1);
			_delay_ms(1000);
			print_info_xbee(XBEE_PRINT_NULL,0);
			return 0;
		}
		timeout_count ++;
		
		
	}
	
	

	
}

// Try a new connection with the server
uint8_t xbee_reconnect(void)
{
	_delay_ms(300);

	if(!xbee_reset_connection())
	{
		SET_ERROR(NETWORK_ERROR);
		print_info_xbee(XBEE_NETWORK_ERROR, 0);
		_delay_ms(300);
		return 1;
	}
	else if (!((xbee.dest_low = xbee_get_address_block(DL_MSG_TYPE)) && (xbee.dest_high = xbee_get_address_block(DH_MSG_TYPE))))
	{
		SET_ERROR(NETWORK_ERROR);
		print_info_xbee(XBEE_NETWORK_ERROR_ADDR , 0);
		_delay_ms(300);
		return 1;
	}
	CLEAR_ERROR(NETWORK_ERROR);		// Successfully reconnected, clear network error
	
	
	_delay_ms(1000);
	return 0;


}

// Start USART0 transmission to XBee module
void xbee_send(uint8_t *data)
{
	send_str_reader = data;						// point to data
	
	USART_IODR = *send_str_reader++;			// Send first data byte (ISR_Tx will do the rest)
	sending_cmd--;								// 1 byte is sent, so decrease counter
	
}

// Send message via XBee module
void xbee_send_msg(uint8_t *buffer, uint8_t length)
{
	if (!length) return;
	
	sending_cmd = length;		//bytes number to send
	xbee_send(buffer);			//send first
	
	_delay_ms(100);
}

// Send message and look for reply from database and copy data to the buffer
uint8_t xbee_send_and_get_reply(uint8_t *buffer, uint8_t length, uint8_t db_cmd_type, uint16_t delay)
{
	uint8_t reply_Id = 0xFF;
	
		//------------------------
		// Answer db_commands are not the same as the requests --> remapping is done here
		uint8_t answer_db_cmd_type;
		switch(db_cmd_type){
			case CMD_send_Ping_95:
			answer_db_cmd_type = CMD_received_Pong_89;
			break;
			//case CMD_send_registration_90:
			//answer_db_cmd_type = CMD_received_set_options_96;
			//break;
			default:
			answer_db_cmd_type = db_cmd_type;
			break;
		}
	

	// if length not 0 get and delete all old replies of this db_cmd_type
	if (length)
	do
	{
		reply_Id = xbee_hasReply(answer_db_cmd_type, EQUAL);	//check for reply
		if (reply_Id != 0xFF)							//reply available
		buffer_removeData(reply_Id);				//mark as read
	}
	while (reply_Id!= 0xFF);

	// send message (only if length not 0) and wait
	xbee_send_msg(buffer, length);
	
	//reset timer!
	uint32_t time_first_try = count_t_elapsed;
	uint8_t delta_t;
	
	// main part
	while(1)
	{
		delta_t = count_t_elapsed - time_first_try;
		if(delta_t > COM_TIMEOUT_TIME)
		{
			break;			//stop trying on timeout return bad reply
		}

		
		// Check for reply
		reply_Id = xbee_hasReply(answer_db_cmd_type, EQUAL);
		_delay_ms(10);
		
		if (reply_Id == 0xFF)
		{//no reply - send again - wait delay ms
			// NO RETRYS
			//xbee_send_msg(buffer, length); no retrys for now
		}
		else
		{//valid reply - mark as read - finish
			frameBuffer[reply_Id].status = 0xFF;											//mark as read
			memcpy(buffer, (uint8_t*)frameBuffer[reply_Id].data, frameBuffer[reply_Id].data_len); 	//copy data to buffer
			return reply_Id;
		}
		
	}
	return 0xFF;
}



uint8_t xbee_send_request_only(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length){
	uint8_t reply_Id = 0xFF;
	

	if(db_cmd_type == CMD_send_Ping_95){
		// Any network error
		print_info_xbee(XBEE_PING, 0);
		}else{
		print_info_xbee(XBEE_SENDING, 0);
	}
	//pack packet
	uint8_t sendbuffer[SINGLE_FRAME_LENGTH];
	
	memcpy(sendbuffer,buffer,length);

	uint8_t temp_bytes_number = xbee_pack_tx64_frame(db_cmd_type, sendbuffer, length, xbee.dest_high, xbee.dest_low);
	
	reply_Id = xbee_send_and_get_reply(sendbuffer, temp_bytes_number, db_cmd_type, 2000);
	
	if(reply_Id == 0xFF)	//request failed!
	{
		print_info_xbee(XBEE_SENDING_ERROR, 0);
		_delay_ms(300);
		SET_ERROR(NO_REPLY_ERROR);
	}
	else {
		memcpy(buffer, (uint8_t*)frameBuffer[reply_Id].data, frameBuffer[reply_Id].data_len);
		
		print_info_xbee(XBEE_SENDING_OK, 0);
		_delay_ms(300);
		
		//TODO this is dangerous --> maybee a per device solution is needed
		CLEAR_ALL();  // Clears all ERRORS
	}
	
	

	
	return reply_Id; //if you want to get reply data afterwards
	//note, that data copied to buffer anyway, delete memcpy and use frameBuffer[index].data[] instead
}



// Send request and receive answer
uint8_t xbee_send_request(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length)
{
	
	
	// =================================================================
	// ========= TRY TO RESOLVE EXISTING NETWORK ERRORS ================
	// =================================================================
	uint8_t reply_Id = 0xFF;
	uint8_t recon_already_tried = 0;

	#ifdef USE_XBEE
	if (  xbee_is_connected() ||  /*CHECK_ERROR(NO_REPLY_ERROR) ||*/ CHECK_ERROR(NETWORK_ERROR) )
	{
		print_info_xbee(XBEE_CHECK_NETWORK, 0);
		
		recon_already_tried = 1;
		
		if (xbee_reconnect()){
			return reply_Id;
		}
	}
	
	#endif
	//===================================================================
	// =============== DEVICE IS SUCCESSFULLY REASSOCIATED ==============
	// ========= OR THERE WAS NO PREEXISTING NETWORK ERROR ==============
	// ==================================================================
	//       !but there might still be an undetected network error!
	
	reply_Id = xbee_send_request_only(db_cmd_type, buffer,  length);
	
	#ifdef USE_XBEE

	if (reply_Id == 0xFF) 	// Transmit Failed
	{
		if (!recon_already_tried){ // Try reconnect then send again
			if (!xbee_reconnect())
			{	// reconn successful
				reply_Id = xbee_send_request_only( db_cmd_type,buffer,  length);
			}
			
			
		}
		
	}
	#endif // USE_XBEE
	
	// ================================================================
	// ====== THE TRANSMISSION FAILED SET ERRORS ACCORDINGLY ==========
	// ================================================================
	if (reply_Id == 0xFF)
	{
		// Network error occurred
		print_info_xbee(XBEE_NO_NETWORK, 0);
		_delay_ms(1000);
		switch(db_cmd_type)
		{
			case FILLING_BEGIN_MSG:
			SET_ERROR(STARTED_FILLING_ERROR);
			break;
			case FILLING_END_MSG:
			SET_ERROR(STOPPED_FILLING_ERROR);
			break;
			case OPTIONS_CHANGED_MSG:
			SET_ERROR(CHANGED_OPTIONS_ERROR);
			break;
			case LONG_INTERVAL_MSG:
			SET_ERROR(SLOW_TRANSMISSION_ERROR);
			break;
			//				case LETTERS_REQUEST:
			//							SET_ERROR(LETTERS_ERROR);
			break;
		}
	}

	return reply_Id;
}


// Send message (and don't receive answer)
uint8_t xbee_send_message(uint8_t db_cmd_type, uint8_t *buffer, uint8_t length)
{

	
	//	LCD_Print("test connection  ", 5, 20, 2, 1, 1, FGC, BGC);

	if (/* !xbee_is_connected() || */ CHECK_ERROR(NETWORK_ERROR) )
	{
		if (CHECK_ERROR(NETWORK_ERROR))
		{
			print_info_xbee(XBEE_NETWORK_ERROR, 0);
			_delay_ms(300);
		}
		print_info_xbee(XBEE_RECONNECTING, 0);

		xbee_reconnect();
	}
	
	if(!CHECK_ERROR(NETWORK_ERROR))
	{
		// Any network error
		#ifdef ALLOW_DEBUG
		print_info_xbee(XBEE_SENDING_MESSAGE, 0);
		#endif

		//pack packet
		uint8_t temp_bytes_number = xbee_pack_tx64_frame(db_cmd_type, buffer, length, xbee.dest_high, xbee.dest_low);
		
		xbee_send_msg(buffer, temp_bytes_number);
		return 1;
	}
	else
	{
		// Network error occurred
		print_info_xbee(XBEE_NO_NETWORK, 0);
		_delay_ms(1000);
		switch(db_cmd_type)
		{
			case FILLING_BEGIN_MSG:
			SET_ERROR(STARTED_FILLING_ERROR);
			break;
			case FILLING_END_MSG:
			SET_ERROR(STOPPED_FILLING_ERROR);
			break;
			case OPTIONS_CHANGED_MSG:
			SET_ERROR(CHANGED_OPTIONS_ERROR);
			break;
			case LONG_INTERVAL_MSG:
			SET_ERROR(SLOW_TRANSMISSION_ERROR);
			break;
			//			case LETTERS_REQUEST:
			//			SET_ERROR(LETTERS_ERROR);
			break;
		}
		return 0;
	}
}

_Bool xbee_get_server_adrr(void){
	xbee.dest_low = xbee_get_address_block(DL_MSG_TYPE);
	xbee.dest_high = xbee_get_address_block(DH_MSG_TYPE);
	
	if (xbee.dest_low && xbee.dest_high)
	{
		return true;
	}
	
	return false;
}


void delay_ms(uint16_t period)	 //delay routine (milliseconds)
{
	for(unsigned int i=0; i<=period; i++)
	_delay_ms(1);
}
