// xbee_utilities.c - Copyright 2016, HZB, ILL/SANE & ISIS
#define RELEASE_XBEE_UTILS 1.00

// HISTORY -------------------------------------------------------------
// 1.00 - First release on June 2016

#include <avr/io.h>
#include <string.h>




#include "xbee.h"
#include "xbee_utilities.h"
#include "xbee_AT_comm.h"
#include "module_globals.h"



// Global Variables
volatile BuffType *setPtr;
volatile BuffType frameBuffer[BUFFER_LENGTH];
volatile uint8_t bufferSize;









//
void buffer_init(void)
{
	setPtr = (BuffType*)&frameBuffer; 	// Set up the IN pointer to the start of the buffer

	bufferSize = 0;           			// Reset the buffer elements counter
}

void buffer_storeData(BuffType data)
{
	if(bufferSize < BUFFER_LENGTH)
	bufferSize++;
	
	*setPtr = data;   							// store data
	setPtr++;									// increment the IN pointer to the next element
	
	if (setPtr == (BuffType*)&frameBuffer[BUFFER_LENGTH])
	setPtr = (BuffType*)&frameBuffer; 	// Wrap pointer if end of array reached
}

// Clear buffer status
void buffer_removeData(uint8_t index)
{
	frameBuffer[index].status = 0xFF;		//bad frame
}

// Return the index of reply corresponding to the command type
uint8_t xbee_hasReply(uint8_t cmd_type, uint8_t range)
{
	switch(range)
	{
		case EQUAL:
		for(uint8_t i = 0; i < bufferSize; ++i)
		{
			if((frameBuffer[i].status == 0x00) && (frameBuffer[i].type == cmd_type))
			{
				return i;
			}
		}
		break;
		// 		case LESS_THAN:
		// 			for(uint8_t i = 0; i < bufferSize; ++i)
		// 			{
		// 				if((frameBuffer[i].status == 0x00) && (frameBuffer[i].type < cmd_type))
		// 				{
		// 					return i;
		// 				}
		// 			}
		// 			break;
		case GREATER_THAN:
		for(uint8_t i = 0; i < bufferSize; ++i)
		{
			if((frameBuffer[i].status == 0x00) && (frameBuffer[i].type > cmd_type))
			{
				return i;
			}
		}
		break;
	}
	return 0xFF;
}

// ...
inline uint8_t get_at_frame_type(char *id, uint8_t len)
{
	//if(len < 2) return UNDEFINED;
	if((id[0] == 'D') && (id[1] == 'A')) return DA_MSG_TYPE;
	if((id[0] == 'D') && (id[1] == 'L')) return DL_MSG_TYPE;
	if((id[0] == 'D') && (id[1] == 'H')) return DH_MSG_TYPE;
	if((id[0] == 'A') && (id[1] == 'I')) return AI_MSG_TYPE;
	if((id[0] == 'H') && (id[1] == 'V')) return HV_MSG_TYPE;
	if((id[0] == 'S') && (id[1] == 'L')) return SL_MSG_TYPE;
	if((id[0] == 'A') && (id[1] == 'S')) return AS_MSG_TYPE;
	if((id[0] == 'S') && (id[1] == 'C')) return SC_MSG_TYPE;
	if((id[0] == 'J') && (id[1] == 'V')) return JV_MSG_TYPE;
	if((id[0] == 'S') && (id[1] == 'D')) return SD_MSG_TYPE;
	if((id[0] == 'V') && (id[1] == 'R')) return VR_MSG_TYPE;
	if((id[0] == 'W') && (id[1] == 'R')) return WR_MSG_TYPE;
	if((id[0] == 'N') && (id[1] == 'I')) return NI_MSG_TYPE;
	if((id[0] == 'C') && (id[1] == 'E')) return CE_MSG_TYPE;
	if((id[0] == 'S') && (id[1] == 'M')) return SM_MSG_TYPE;
	if((id[0] == 'C') && (id[1] == 'H')) return CH_MSG_TYPE;
	if((id[0] == 'Z') && (id[1] == 'S')) return ZS_MSG_TYPE;
	if((id[0] == 'N') && (id[1] == 'J')) return NJ_MSG_TYPE;
	if((id[0] == 'A') && (id[1] == '1')) return A1_MSG_TYPE;
	if((id[0] == 'A') && (id[1] == '2')) return A2_MSG_TYPE;
	if((id[0] == 'D') && (id[1] == 'B')) return DB_MSG_TYPE;
	
	return 0;
}

// Build a frame for XBee communication
inline void xbee_build_frame(uint8_t *buffer, uint8_t length)
{
	if(length < 5) return;
	if(!xbee_chkChecksum(buffer)) return;
	
	BuffType newFrame;
	uint8_t data_counter = 0;
	
	newFrame.length = (buffer[1]<<8) + buffer[2];
	newFrame.api_id = buffer[3];
	newFrame.data_len = 0;	//init
	newFrame.type = 0;		//init
	
	#ifdef USE_LAN
	switch (newFrame.api_id)
	{
		case API_TX_64: //0x00
		newFrame.type = buffer[14];
		newFrame.status = STATUS_BYTE;// buffer[15];
		
		if (length < (DATA_INDEX+1)) return;
		for (uint8_t i = DATA_INDEX; i < length; ++i)
		newFrame.data[data_counter++] = buffer[i];
		newFrame.data_len = --data_counter;
		
		if (newFrame.type == (SET_OPTIONS_CMD||LOGIN_MSG))
		{
			
			GCM_IP.IP_oct_1 = buffer[5];
			GCM_IP.IP_oct_2= buffer[7];
			GCM_IP.IP_oct_3 = buffer[9];
			GCM_IP.IP_oct_4 = buffer[11];
		}
		
		break;
		
		case API_AT_CMD: //0x08
		
		if (buffer[4] && (buffer[5] == (uint8_t) 'C') && (buffer[6] == (uint8_t) 'E'))
		{
			newFrame.type = SIMULATE_XBEE_CMD;
			newFrame.status = 0;
		}
		else
		{
			newFrame.status = 0xFF;
		}
		break;
		
		default:
		
		return;
		break;
		
	}

	#endif
	
	#ifdef USE_XBEE
	switch(newFrame.api_id)
	{
		case AT_ID:
		newFrame.status = buffer[7];
		newFrame.type = get_at_frame_type((char*) &buffer[5], 2);
		
		//write data
		if (length < (8+1)) return;
		for (uint8_t i = 8; i < length; ++i)
		newFrame.data[data_counter++] = buffer[i];
		newFrame.data_len = --data_counter;
		break;
		case RX_ID:
		
		newFrame.type = buffer[OPCODE_INDEX];
		newFrame.status = STATUS_BYTE;
		
		if (length < (DATA_INDEX+1)) return;
		for (uint8_t i = DATA_INDEX; i < length; ++i)
		newFrame.data[data_counter++] = buffer[i];
		newFrame.data_len = --data_counter;
		break;
		case STA_ID:
		if (buffer[4]==2)
		{
			xbee.associated = 1;
			
			}else if(buffer[4]==3){
			xbee.associated = 0;
		}
		newFrame.status =  0xFF;
		newFrame.type = STATUS_MSG_TYPE;
		newFrame.data[0]= buffer[4];
		newFrame.data_len =1;
		break;
		case AT_REMOTE_ID:


		
		newFrame.status = buffer[17];
		newFrame.type = get_at_frame_type((char*) &buffer[15],2);
		
		//write data
		if (length < (18+1)) return;
		for (uint8_t i = 18; i < length; ++i)
		newFrame.data[data_counter++] = buffer[i];
		newFrame.data_len = --data_counter;

		break;
		
		default:
		newFrame.status = 0xFF;
	}
	#endif
	
	buffer_storeData(newFrame);
}

// Store device position (instrument id or labs) into the buffer
inline uint8_t devicePos_to_buffer(char *pos, uint8_t offset, uint8_t *buffer)
{
	uint8_t index = 0;
	
	while (pos[index]!='\0')
	{
		buffer[index+offset] = pos[index];
		index++;
	}
	
	//fill spaces if pos is less then 4 letters
	for(uint8_t i=0; i<(4 - index); i++) buffer[i+index+offset] = 0x20;
	
	//return index, add spaces
	return (index + offset) + (4 - index);
}

// Pack data (string + int parameters) in a XBee-frame (AT) ready to send from frame_buffer
// Only used with xbee_reset_connection & xbee_getAddress functions
uint8_t xbee_pack_tx_frame(uint8_t *params, uint8_t paramsNumber)
{
	uint8_t temp_buffer[SINGLE_FRAME_LENGTH];
	uint8_t index = 5;
	
	uint8_t *temp = params;

	temp_buffer[0] = 0x7E;		// Start delimiter
	// Let index 1 & 2 free for storing frame length
	temp_buffer[3] = 0x08;   	// API Identifier Value for AT Command type allows for module parameters to be queried or set.
	temp_buffer[4] = 0x42;		// Constant Frame ID arbitrarily selected / Different of 0 to get an answer
	
	while (paramsNumber)
	{
		temp_buffer[index++] = *params++;
		paramsNumber--;
	}
	
	temp_buffer[index] = xbee_getChecksum(temp_buffer,3,index);		// Calculate checksum
	
	temp_buffer[1] = ((index-3) >> 8);
	temp_buffer[2] = (index-3);
	
	memcpy(temp, temp_buffer, index+1);		// Store frame in the params array
	
	return index+1;		// Return number of bytes packed
}
// Function provided for convenience.
// When using this function you will able to send the data properly only once.
// Pure data passed in frame_buffer will be packed in frame_buffer.

// Pack data (string + int parameters) in a XBee-frame (TX 64-bit address) ready to send
// Disable acknowledgment and response frame
// Returns number of bytes packed
uint8_t xbee_pack_tx64_frame(uint8_t db_cmd_type, uint8_t *params, uint8_t paramsNumber, uint32_t dest_high, uint32_t dest_low)
{
	uint8_t temp_buffer[SINGLE_FRAME_LENGTH];
	uint8_t index = 0;
	
	uint8_t *temp = params;
	
	#ifdef USE_XBEE

	temp_buffer[0] = 0x7E;		// Start delimiter
	// Let index 1 & 2 free for storing frame length
	temp_buffer[3] = 0x00;   	// API Identifier Value for 64-bit TX Request message will cause the module to send RF Data as an RF Packet.
	temp_buffer[4] = 0x00;		// Constant Frame ID arbitrarily selected
	// Identifies the UART data frame for the host to correlate with a subsequent ACK (acknowledgment).
	// Setting Frame ID to ‘0' will disable response frame.
	
	// 32-bit destination address high
	temp_buffer[5] = (uint8_t)(dest_high >> 24);
	temp_buffer[6] = (uint8_t)(dest_high >> 16);
	temp_buffer[7] = (uint8_t)(dest_high >> 8);
	temp_buffer[8] = (uint8_t) dest_high;
	
	// 32-bit destination address low
	temp_buffer[9] = (uint8_t)(dest_low >> 24);
	temp_buffer[10] = (uint8_t)(dest_low >> 16);
	temp_buffer[11] = (uint8_t)(dest_low >> 8);
	temp_buffer[12] = (uint8_t) dest_low;
	
	temp_buffer[13] = 0x01;				// Disable acknowledgment
	temp_buffer[14] = db_cmd_type;		// Database command type
	
	#endif
	
	#ifdef USE_LAN
	temp_buffer[0] = 0x7E;
	//len
	temp_buffer[3] = 0x80;   	//API ID 64 bit RX // must be together 0x80 to be compatible with the xBee frame structure for the server
	
	temp_buffer[4] = GCM_IP.IP_oct_1;
	temp_buffer[5] = 46;
	temp_buffer[6] = GCM_IP.IP_oct_2;
	temp_buffer[7] = 46;
	
	temp_buffer[8] = GCM_IP.IP_oct_3;
	temp_buffer[9] = 46;
	temp_buffer[10] = GCM_IP.IP_oct_4;
	temp_buffer[11] = 0;
	
	temp_buffer[12] = 0x00;
	temp_buffer[13] = 0x03;
	temp_buffer[14] = db_cmd_type;
	
	
	#endif
	
	temp_buffer[15] = MESSAGEFOPRMAT_IDENTIFIER;
	temp_buffer[16] = version.Branch_id;
	temp_buffer[17] = (uint8_t)( version.Fw_version>> 8); //MSB
	temp_buffer[18] = (uint8_t) (version.Fw_version );  //LSB

	
	#ifdef LEVELMETER
	// Pack entered device number in buffer
	for(uint8_t i = 0; i < xbee.dev_id_str_len; i++){
		temp_buffer[i+19] = xbee.dev_id_str[i];
	}
	
	index = 19 + xbee.dev_id_str_len;
	#else
	
	index = 19;
	#endif
	

	
	

	// Parameter
	while (paramsNumber)
	{
		temp_buffer[index] = *params++;
		index++;
		paramsNumber--;
	}
	
	// Calculate checksum
	temp_buffer[index] = xbee_getChecksum(temp_buffer,3,index);
	
	// Add frame length - excludes Start delimiter, Length and checksum
	temp_buffer[1] = ((index-3) >> 8);
	temp_buffer[2] = (index-3);
	
	memcpy(temp, temp_buffer, index+1);		// Frame is in the params array
	
	return index+1;
}

// Calculate checksum of the packed frame
uint8_t xbee_getChecksum(uint8_t *buffer, uint8_t start, uint8_t stop)
{
	if (start > stop) return 0;
	
	uint8_t summe = 0;
	uint8_t index = start;
	
	while(index < stop)
	{//add all bytes from start to stop
		summe += buffer[index++];
	}
	return 0xFF-summe;//result;
}

// Return true if checksum is correct, false otherwise
_Bool xbee_chkChecksum(uint8_t *buffer)
{
	uint8_t index = 0;
	uint8_t summe = 0;
	uint16_t bytes_number = xbee_get_packet_len(buffer)+1; //data + checksum
	
	while(bytes_number)
	{//add all bytes after frame length (3.byte)
		summe += buffer[index+3];
		bytes_number--;
		index++;
	}
	return (summe == 0xFF);
}

// Return the length of the passed XBee-frame
uint8_t xbee_get_packet_len(uint8_t *buffer)
{
	return (buffer[1]<<8) + buffer[2];
}



/**
* @brief Used for the LAN-Variant of the Gascounters. Necessary for Xbee simulation.
*
* @param CMD_NAME_FC
* @param CMD_NAME_SC
* @param status
* @param values
* @param value_length
*
* @return void
*/
void xbee_pseudo_send_AT_response( char CMD_NAME_FC, char CMD_NAME_SC, uint8_t status, uint8_t *values, uint8_t value_length)
{


	uint8_t temp_buffer[50];
	uint16_t i;
	
	uint8_t *temp = values;
	
	temp_buffer[0] = 0x7E;
	//length
	temp_buffer[3] = API_AT_CMD_RESPONSE;
	temp_buffer[4] = 1;// FRAME_ID;
	temp_buffer[5] = (uint8_t) CMD_NAME_FC;
	temp_buffer[6] = (uint8_t) CMD_NAME_SC;
	temp_buffer[7] = status;
	
	i = 8;
	
	while (value_length)
	{
		temp_buffer[i++] = *values++;
		value_length--;
	}
	
	//checksum
	temp_buffer[i] = xbee_getChecksum(temp_buffer,3,i);
	
	//add length
	temp_buffer[1] = ((i-3) >> 8);
	temp_buffer[2] = (i-3);
	
	memcpy(temp, temp_buffer, i+1);
	xbee_send_msg(temp,i+1);
	
	
}
