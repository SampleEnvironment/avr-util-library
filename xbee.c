// xbee.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_XBEE 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on May 2016

//#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>

#include "../config.h"


#include "usart.h"
#include "xbee_utilities.h"
#include "xbee.h"
#include "xbee_AT_comm.h"
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
	.associated = 0,
	.scanDurarion = 16.0,
	.ScanChannels = SC_MASK_DEFAULT,
	.ScanChannels_current = SC_MASK_DEFAULT,
	.CoordIdentifier = "",
	.netstat = ONLINE,
	.RSSI = 0
	
};

void xbee_init(void (*printInfoFun)(char *,_Bool),char * dev_ID_str,uint8_t max_devid_str_len,uint16_t SC_mask){
	print_info_xbee = printInfoFun;
	print_info_AT   = printInfoFun;
	xbee.dev_id_str = dev_ID_str;
	xbee.dev_id_str_len = max_devid_str_len;
	xbee.ScanChannels = SC_mask;
	xbee.ScanChannels_current = SC_mask;
	//xbee_Set_Scan_Channels(CHANNEL_MASK);
	//xbee_WR();

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

char* xbee_get_coordID(void){
	return xbee.CoordIdentifier;
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



// Reset connection with the xbee coordinator and initiate a new one
// Returns true if reconnection is successful, false otherwise
_Bool xbee_reset_connection(uint8_t force)
{

	// Clear frame_buffer if there is still some information in the queue
	buffer_init();

	if (force)
	{
		if(!xbee_DA_initiate_reassociation()) return 0;
	}else if(!xbee_coordIdentifier())
	{
		if(!xbee_DA_initiate_reassociation()) return 0;
	}
	
	




	
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
uint8_t xbee_reconnect(uint8_t force_DA)
{
	// IF there is no network reset SC MASK
	if (xbee.ScanChannels != xbee.ScanChannels_current && xbee.netstat == NO_NETWORK)
	{
		
		xbee_Set_Scan_Channels(xbee.ScanChannels);
		xbee_WR();
		print_info_xbee(XBEE_RESET_SC,1);
		_delay_ms(2000);
	}
	
	_delay_ms(300);

	if(!xbee_reset_connection(force_DA))
	{
		SET_ERROR(NETWORK_ERROR);
		print_info_xbee(XBEE_NETWORK_ERROR, 0);
		xbee.netstat = NO_NETWORK;

		_delay_ms(300);
		return 1;
	}
	else if (!((xbee.dest_low = xbee_get_address_block(DL_MSG_TYPE)) && (xbee.dest_high = xbee_get_address_block(DH_MSG_TYPE))))
	{
		SET_ERROR(NETWORK_ERROR);
		print_info_xbee(XBEE_NETWORK_ERROR_ADDR , 0);
		_delay_ms(300);
		xbee_get_DB();
		return 1;
	}
	CLEAR_ERROR(NETWORK_ERROR);		// Successfully reconnected, clear network error

	
	_delay_ms(1000);
	xbee_get_DB();
	xbee_coordIdentifier();
	return 0;


}

// Start USART0 transmission to XBee module
void xbee_send(uint8_t *data)
{
	USART0.send_str_reader = data;						// point to data
	
	USART_IODR = *USART0.send_str_reader++;			// Send first data byte (ISR_Tx will do the rest)
	USART0.sending_cmd--;								// 1 byte is sent, so decrease counter
	
}

// Send message via XBee module
void xbee_send_msg(uint8_t *buffer, uint8_t length)
{
	if (!length) return;
	
	USART0.sending_cmd = length;		//bytes number to send
	xbee_send(buffer);			//send first
	
	_delay_ms(100);
}

// Send message and look for reply from database and copy data to the buffer
uint8_t xbee_send_and_get_reply(uint8_t *buffer, uint8_t length, uint8_t db_cmd_type, uint16_t comTimeOut)
{
	uint8_t reply_Id = 0xFF;
	


	// if length not 0 get and delete all old replies of this db_cmd_type
	if (length)
	do
	{
		reply_Id = xbee_hasReply(db_cmd_type, EQUAL);	//check for reply
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
		if(delta_t > comTimeOut)
		{
			break;			//stop trying on timeout return bad reply
		}

		
		// Check for reply
		reply_Id = xbee_hasReply(db_cmd_type, EQUAL);
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
	

	if(db_cmd_type == PING_MSG){
		// Any network error
		print_info_xbee(XBEE_PING, 0);
		}else{
		print_info_xbee(XBEE_SENDING, 0);
	}
	_delay_ms(100);
	//pack packet
	uint8_t sendbuffer[SINGLE_FRAME_LENGTH];
	
	memcpy(sendbuffer,buffer,length);

	uint8_t temp_bytes_number = xbee_pack_tx64_frame(db_cmd_type, sendbuffer, length, xbee.dest_high, xbee.dest_low);
	
	reply_Id = xbee_send_and_get_reply(sendbuffer, temp_bytes_number, db_cmd_type, COM_TIMEOUT_TIME);
	
	if(reply_Id == 0xFF)	//request failed!
	{
		print_info_xbee(XBEE_SENDING_ERROR, 0);
		_delay_ms(300);
		SET_ERROR(NO_REPLY_ERROR);
	}
	else {
		memcpy(buffer, (uint8_t*)frameBuffer[reply_Id].data, frameBuffer[reply_Id].data_len);
		
		if(db_cmd_type == PING_MSG){
			print_info_xbee(XBEE_PING_OK, 0);
			_delay_ms(300);
			}else{
			print_info_xbee(XBEE_SENDING_OK, 0);
			_delay_ms(300);
		}
		_delay_ms(300);
		
		xbee.netstat = ONLINE;
		CLEAR_ALL();  // Clears all newtwork related ERRORS
	}
	
	

	xbee_get_DB();
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
	
	#ifdef USE_XBEE
	uint8_t network_up = 0;
	uint8_t recon_already_tried = 0;
	
	// IF there is no network reset SC MASK
	if (xbee.ScanChannels != xbee.ScanChannels_current && xbee.netstat == NO_NETWORK)
	{
		
		xbee_Set_Scan_Channels(xbee.ScanChannels);
		xbee_WR();
		print_info_xbee(XBEE_RESET_SC,1);
		_delay_ms(2000);
	}
	

	
	if (  xbee_is_connected() ||  /*CHECK_ERROR(NO_REPLY_ERROR) ||*/ CHECK_ERROR(NETWORK_ERROR) )
	{
		print_info_xbee(XBEE_CHECK_NETWORK, 0);
		
		recon_already_tried = 1;
		
		if (xbee_reconnect(0)){
			// No coordinator available --> no network
			print_info_xbee(XBEE_NO_NETWORK,0);
			xbee.netstat = NO_NETWORK;
			xbee_get_DB();
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
			if (!xbee_reconnect(0))
			{	// reconn successful
				network_up = 1;
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
		#ifdef USE_XBEE

		if (network_up)
		{
			// ===========================================================
			// Try a different coordinator (switch xbee operating channel)
			// ===========================================================
			print_info_xbee(XBEE_NO_SERV,0);
			xbee.netstat = NO_SERVER;
			
			_delay_ms(1000);
			print_info_xbee(XBEE_SWITCH_CHANNEL,0);
			xbee_clear_Curr_Channel_from_SC();
			xbee_WR();

			


			if (!xbee_reconnect(1))
			{	// reconn successful
				// new coordinator found
				reply_Id = xbee_send_request_only( db_cmd_type,buffer,  length);
			}
			else
			{
				// revert back to old Coordinator
				xbee_Set_Scan_Channels(xbee.ScanChannels);
				xbee_WR();
				
				if (xbee_reconnect(1))
				{
					//old Coordinator still available
					xbee.netstat = NO_SERVER;
					xbee_get_DB();
				}else{
					//old Coordinator not available
					xbee.netstat = NO_NETWORK;
				}
			

			}
			
			if (reply_Id != 0xFF)
			{
				xbee.netstat = ONLINE;
				xbee_coordIdentifier();
				return reply_Id;
			}

			
		}
		else
		{
			print_info_xbee(XBEE_NO_NETWORK, 0);
			xbee.netstat = NO_NETWORK;
			_delay_ms(500);
			// reset Scan Mask
			print_info_xbee(XBEE_RESET_SC,0);
			xbee_Set_Scan_Channels(xbee.ScanChannels);
			xbee_WR();
			_delay_ms(500);
		}
		#endif
		
		#ifdef USE_LAN
		xbee.netstat = NO_NETWORK;
		#endif
		
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
		}else{
		xbee.netstat = ONLINE;
	}
	//_delay_ms(1000);
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

		xbee_reconnect(0);
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
		xbee_get_DB();
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
