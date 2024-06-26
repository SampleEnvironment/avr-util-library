/*
* xbee_AT_comm.c
*
* Created: 16.02.2022 09:40:12
*  Author: qfj
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../config.h"
#include "xbee_AT_comm.h"
#include "xbee.h"
#include "xbee_utilities.h"
#include "heap.h"




void (*print_info_AT)(char *, _Bool )  = NULL;


PanPoolType Pans;


char AT_Lut[22] [2] = {
	{'D','A'},
	{'D','H'},
	{'D','L'},
	{'0','0'},
	{'A','I'},
	{'H','V'},
	{'S','L'},
	{'A','S'},
	{'S','C'},
	{'J','V'},
	{'S','D'},
	{'V','R'},
	{'W','R'},
	{'N','I'},
	{'C','E'},
	{'S','M'},
	{'C','H'},
	{'Z','S'},
	{'N','J'},
	{'A','1'},
	{'A','2'},
	{'D','B'}
	
};



AT_commandType* initAt_set(AT_commandType * Atcommand,AT_MESSAGE AT_Code,uint8_t * data,uint8_t data_len){
	
	Atcommand->MSG_TYPE = AT_Code;
	
	Atcommand->MSC_AT_CODE = AT_Lut[(uint8_t)(AT_Code-AT_START)][0];
	Atcommand->LSC_AT_CODE = AT_Lut[(uint8_t)(AT_Code-AT_START)][1];
	
	
	if (data != NULL)
	{
		memcpy(Atcommand->data,data,sizeof(uint8_t)*data_len);
	}

	Atcommand->data_len = data_len;
	
	Atcommand->AnswerReceived = false;
	
	
	return Atcommand;
}

AT_commandType* initAt_read(AT_commandType * Atcommand,AT_MESSAGE AT_Code){
	
	Atcommand->MSG_TYPE = AT_Code;
	
	Atcommand->MSC_AT_CODE = AT_Lut[(uint8_t)(AT_Code-AT_START)][0];
	Atcommand->LSC_AT_CODE = AT_Lut[(uint8_t)(AT_Code-AT_START)][1];
	
	Atcommand->data_len = 0;
	
	Atcommand->AnswerReceived = false;
	
	
	return Atcommand;
}


uint8_t send_remoteAT(AT_commandType *AT_Command){
	uint8_t buffer[SINGLE_FRAME_LENGTH];
	
	uint8_t temp_bytes_number = xbee_pack_remoteAT_frame(AT_Command,buffer);
	
	uint8_t reply_Id = xbee_send_and_get_reply(buffer, temp_bytes_number,AT_Command->MSG_TYPE, 1);
	
	if (reply_Id != 0xFF)
	{

		memcpy(AT_Command->data,(uint8_t *)frameBuffer[reply_Id].data,frameBuffer[reply_Id].data_len);
		
		AT_Command->data_len = frameBuffer[reply_Id].data_len;
		AT_Command->AnswerReceived = true;
	}
	
	return reply_Id;
}


void send_AT_noRep(AT_commandType *AT_Command){
	uint8_t buffer[SINGLE_FRAME_LENGTH];
	
	buffer[0] = (uint8_t) AT_Command->MSC_AT_CODE;
	buffer[1] = (uint8_t) AT_Command->LSC_AT_CODE;
	
	memcpy(&buffer[2],AT_Command->data,sizeof(uint8_t)*AT_Command->data_len);

	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, AT_Command->data_len+2);
	
	// send message (only if length not 0) and wait
	xbee_send_msg(buffer, temp_bytes_number);
}


uint8_t send_AT(AT_commandType *AT_Command){
	uint8_t buffer[SINGLE_FRAME_LENGTH];
	
	buffer[0] = (uint8_t) AT_Command->MSC_AT_CODE;
	buffer[1] = (uint8_t) AT_Command->LSC_AT_CODE;
	
	memcpy(&buffer[2],AT_Command->data,sizeof(uint8_t)*AT_Command->data_len);

	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, AT_Command->data_len+2);
	
	uint8_t reply_Id = xbee_send_and_get_reply(buffer, temp_bytes_number,AT_Command->MSG_TYPE, 4);
	
	if (reply_Id != 0xFF)
	{
		switch(frameBuffer[reply_Id].data_len){
			case 1: // One Byte
			AT_Command->byte = frameBuffer[reply_Id].data[0];
			break;
			
			case 2: // Data Word
			AT_Command->dataword = (frameBuffer[reply_Id].data[0] << 8) + frameBuffer[reply_Id].data[1];
			
			break;
			
			case 4: // uint32
			AT_Command->Address = (unsigned long int)frameBuffer[reply_Id].data[0]<<24;
			AT_Command->Address += (unsigned long int)frameBuffer[reply_Id].data[1]<<16;
			AT_Command->Address += (unsigned long int)frameBuffer[reply_Id].data[2]<<8;
			AT_Command->Address += (unsigned long int)frameBuffer[reply_Id].data[3];
			break;
			

			case 22: // Pan Descriptor
			memcpy(&AT_Command->pandesc,(uint8_t *)frameBuffer[reply_Id].data,sizeof(PanDescriptor_15_4));
			if (frameBuffer[reply_Id].type == AS_MSG_TYPE)
			{
				break;
			}
			default:
			memcpy(AT_Command->data,(uint8_t *)frameBuffer[reply_Id].data,frameBuffer[reply_Id].data_len);
			break;
			
		}
		
		
		
		AT_Command->data_len = frameBuffer[reply_Id].data_len;
		AT_Command->AnswerReceived = true;
	}
	
	return reply_Id;
	
}

uint16_t xbee_get_Scan_duration(void){
	
	#ifdef USE_XBEE

	
	AT_commandType AT_command;
	
	initAt_read(&AT_command,SD_MSG_TYPE);
	
	send_AT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 0;
	}
	if (AT_command.byte != 0)
	{
		//
		double nChannels = countSetBits(xbee.ScanChannels_current);
		xbee.scanDurarion = (((nChannels * (2 << AT_command.byte) * 16) + (38 * nChannels) + 20) / 1000)+1;
		
		return 1;
	}
	
	return 0;
	
	#endif
	
	#ifdef USE_LAN
	return 0;
	#endif
	
}


uint8_t xbee_get_DB(void)
{
	
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,DB_MSG_TYPE);
	
	send_AT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 0;
	}
	if (AT_command.byte != 0)
	{
		xbee.RSSI = AT_command.byte;
		
		return 1;
		
		
	}
	
	return 0;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
	
}


uint8_t xbee_set_Scan_Duration(uint8_t dur_exp){
	
	
	#ifdef USE_XBEE

	
	AT_commandType AT_command;

	initAt_set(&AT_command,SD_MSG_TYPE,&dur_exp,1);
	
	send_AT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 0;
	}
	return 1;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
	
}

uint8_t xbee_WR(void){
	
	#ifdef USE_XBEE

	AT_commandType AT_command;

	initAt_read(&AT_command,WR_MSG_TYPE);
	
	send_AT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 0;
	}
	return 1;
	#endif


	#ifdef USE_LAN
	return 1;
	#endif
}

uint8_t xbee_coordIdentifier(void){
	#ifdef USE_XBEE
	char tempold[21];
	strcpy(xbee.CoordIdentifier,tempold);
	AT_commandType AT_command;
	
	initAt_read(&AT_command,NI_MSG_TYPE);
	
	send_remoteAT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		char temp[21] = "NO NETWORK";
		strncpy(xbee.CoordIdentifier,temp,sizeof(char)* 11);
		return 0;

	}
	

	memcpy(xbee.CoordIdentifier,AT_command.data,AT_command.data_len*sizeof(char));
	xbee.CoordIdentifier[AT_command.data_len] = '\0'; // null terminator anf�gen
	
	
	xbee.CoordIdChanged = (!strcmp(tempold,xbee.CoordIdentifier))?true:false;

	
	
	return 1;
	
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}



uint32_t countSetBits(uint32_t n)
{
	uint32_t count = 0;
	while (n) {
		n &= (n - 1);
		count++;
	}
	return count;
}


uint8_t AT_read_Byte( AT_MESSAGE AT_Code){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,AT_Code);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	return AT_command.byte;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}


uint8_t AT_set_Byte( AT_MESSAGE AT_Code,uint8_t param){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_set(&AT_command,AT_Code,&param,1);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return 1;
	
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}

uint8_t xbee_is_connected(void)
{
	#ifdef USE_XBEE


	AT_commandType AT_command;
	
	initAt_read(&AT_command,AI_MSG_TYPE);


	// API command "AI" => Reads possible errors with the last association request, result 0: everything is o.k.
	send_AT(&AT_command);
	
	#ifdef ALLOW_DEBUG
	print_info_xbee(XBEE_AI_MESSAGE, 0);
	_delay_ms(1000);
	#endif
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 1;
	}
	if (AT_command.byte != 0)
	{
		// not associated with Coordinator
		return AT_command.byte;
	}

	//Xbee is associated to a Coordinator
	return 0;

	
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}


// Serial number low
uint32_t xbee_SL_address(void){
	uint32_t 	SL_Address = 0;
	
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,SL_MSG_TYPE);
	send_AT(&AT_command);

	if(AT_command.AnswerReceived == false) return 0;
	
	SL_Address = AT_command.Address;
	

	
	

	#endif // USE_XBEE
	#ifdef USE_LAN

	#endif
	
	return SL_Address;
}

// Get high and low bytes of basis station
// Used only with "DL" and "DH" command type
uint32_t xbee_get_address_block(uint8_t cmd_type)
{
	
	#ifdef USE_XBEE

	_delay_ms(100);
	AT_commandType AT_command;


	
	if(cmd_type != DL_MSG_TYPE && cmd_type != DH_MSG_TYPE)
	{
		// wrong command type
		return 0;
	}
	
	initAt_read(&AT_command,cmd_type);
	send_AT(&AT_command);
	
	
	if (AT_command.AnswerReceived == true)
	{
		
		return AT_command.Address;
	}
	else return 0;		// Couldn't read addr_high or addr_low
	
	#endif

	#ifdef USE_LAN
	return 0;
	#endif
}

uint8_t xbee_DA_initiate_reassociation(void){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,DA_MSG_TYPE);
	send_AT(&AT_command);

	if(AT_command.AnswerReceived == false) return 0;
	
	return 1;
	
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}

uint8_t xbee_JV_verification(void){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,JV_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	return AT_command.byte;
	
	#endif
	
	#ifdef USE_LAN
	return 0;
	#endif
}

uint8_t xbee_Node_Join(void){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,NJ_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	return AT_command.byte;
	
	#endif
	
	#ifdef USE_LAN
	return 0;
	#endif
}

uint8_t xbee_set_Node_Join(uint8_t NJ){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_set(&AT_command,CE_MSG_TYPE,&NJ,1);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return 1;
	
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}





uint8_t xbee_stack_profile(void){
	#ifdef USE_XBEE
	
	AT_commandType AT_command;
	
	initAt_read(&AT_command,ZS_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	return AT_command.byte;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}

uint8_t xbee_channel(void){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_read(&AT_command,CH_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0xff;
	
	return AT_command.byte;
	
	#endif
	
	#ifdef USE_LAN
	return 0xff;
	#endif
}


uint16_t xbee_hardware_version(void){
	uint16_t hw_version_16 = 0;
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,HV_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	hw_version_16 = AT_command.dataword ;
	
	
	version.hw_version_xbee = (hw_version_16 > 0x2000)? XBEE_V_SC2 : XBEE_V_S1;
	
	#endif // USE_XBEE
	#ifdef USE_LAN
	version.hw_version_xbee = XPORT;
	#endif
	
	return hw_version_16;
}

uint16_t xbee_firmware_version(void){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,VR_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return AT_command.dataword ;

	
	#endif // USE_XBEE
	
	
	#ifdef USE_LAN
	return 0;
	#endif
	

}

uint8_t xbee_coordinator_Enable(void){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_read(&AT_command,CE_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0xFF;
	
	uint8_t CE = AT_command.byte ;
	
	
	return CE;
	
	#endif
	
	#ifdef USE_LAN
	return 0xFF;
	#endif
	
}

uint8_t xbee_set_coordinator_Enable(uint8_t CE){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_set(&AT_command,CE_MSG_TYPE,&CE,1);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return 1;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
	
}


uint8_t xbee_sleep_Mode(void){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_read(&AT_command,SM_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0xFF;
	
	uint8_t SM = AT_command.byte ;
	
	
	return SM;
	#endif
	
	#ifdef USE_LAN
	return 0xFF;
	#endif
	
}

uint8_t xbee_set_sleep_Mode(uint8_t SM){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_set(&AT_command,SM_MSG_TYPE,&SM,1);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return 1;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
	
}


uint8_t xbee_Assiciation_indication(void){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_read(&AT_command,AI_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0xFF;
	
	uint8_t AI = AT_command.byte ;
	
	
	return AI;
	#endif
	
	#ifdef USE_LAN
	return 0xFF;
	#endif
	
}


uint8_t xbee_clear_Curr_Channel_from_SC(void){
	#ifdef USE_XBEE
	
	// get current operating CH
	uint8_t currCh = xbee_channel();
	
	// default channelmask (to avoid successively removing more and more channels)
	xbee.ScanChannels_current = xbee.ScanChannels;
	
	// remove current channel from SC mask
	xbee.ScanChannels_current &=~(1<<(currCh-0xB));
	
	// set new SC Mask
	return xbee_Set_Scan_Channels(xbee.ScanChannels_current);
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
	
	
}

uint16_t xbee_Scan_Channels(void){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	initAt_read(&AT_command,SC_MSG_TYPE);
	send_AT(&AT_command);

	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	uint16_t ScanChannels = AT_command.dataword ;
	
	xbee.ScanChannels_current = ScanChannels;
	
	return ScanChannels;
	#endif
	
	#ifdef USE_LAN
	return 0;
	#endif
	
}


uint8_t xbee_Set_Scan_Channels(uint16_t SC_Bitfield){
	#ifdef USE_XBEE
	AT_commandType AT_command;
	
	uint8_t buffer[2];
	// xbee uses little endian byte order
	buffer[0] = SC_Bitfield >> 8 ;
	buffer[1] = SC_Bitfield ;
	
	initAt_set(&AT_command,SC_MSG_TYPE,buffer,2);
	send_AT(&AT_command);

	
	if(AT_command.AnswerReceived == false) return 0;
	
	xbee.ScanChannels_current = SC_Bitfield;
	
	return 1;
	#endif
	
	
	#ifdef USE_LAN
	return 1;
	#endif
	
}

uint8_t xbee_Active_Scan(void){
	#ifdef USE_XBEE
	
	
	print_info_AT("Active Scan    ",1);
	
	uint8_t panArr_index = 0;
	uint8_t reply_ID = 0;
	
	AT_commandType AT_command;
	
	Pans.nPans = 0;
	
	xbee_set_Scan_Duration(4);
	
	if(!xbee_get_Scan_duration()){
		print_info_AT("get SD Failed",1);
		return 0;
	}
	
	
	resetHeap(&Pans.Heap);
	
	initAt_read(&AT_command,AS_MSG_TYPE);
	send_AT_noRep(&AT_command);

	
	uint32_t time_first_try = count_t_elapsed;
	uint8_t delta_t;
	
	
	while(1)
	{
		delta_t = count_t_elapsed - time_first_try;
		if(delta_t > xbee.scanDurarion)
		{
			break;			//stop trying on timeout return bad reply
		}

		reply_ID = xbee_hasReply(AS_MSG_TYPE, EQUAL);	//check for reply
		_delay_ms(100);
		
		if (reply_ID != 0xFF){							//reply available
			panArr_index = addFrameToPanPool(reply_ID,panArr_index);
			
			buffer_removeData(reply_ID);				    //mark as read
		}
		else
		{
			continue;
		}

		

		
	}
	
	print_info_AT("",0);
	Pans.nPans = panArr_index;
	return 1;
	#endif
	
	#ifdef USE_LAN
	return 1;
	#endif
}

uint8_t addFrameToPanPool(uint8_t reply_ID,uint8_t panArrIndex){
	
	if (frameBuffer[reply_ID].data_len == 22)
	{
		
		//memcpy(&Pans.Pool[panArrIndex],(uint8_t *)frameBuffer[reply_ID].data,sizeof(PanDescriptor_15_4));

		
		Pans.Pool[panArrIndex].CoordSerialNumber =   (
		((uint64_t) frameBuffer[reply_ID].data[0] << 56) |
		((uint64_t) frameBuffer[reply_ID].data[1] << 48) |
		((uint64_t) frameBuffer[reply_ID].data[2] << 40) |
		((uint64_t) frameBuffer[reply_ID].data[3] << 32) |
		((uint64_t) frameBuffer[reply_ID].data[4] << 24) |
		((uint64_t) frameBuffer[reply_ID].data[5] << 16) |
		((uint64_t) frameBuffer[reply_ID].data[6] << 8) |
		((uint64_t) frameBuffer[reply_ID].data[7]  ));
		
		Pans.Pool[panArrIndex].PanID= (
		((uint16_t) frameBuffer[reply_ID].data[8] << 8) |
		((uint16_t) frameBuffer[reply_ID].data[9] ));
		
		Pans.Pool[panArrIndex].ChannelID = frameBuffer[reply_ID].data[11];
		
		Pans.Pool[panArrIndex].RSSI = frameBuffer[reply_ID].data[18];
		


		
		push(&Pans.Heap,(uint8_t) (Pans.Pool[panArrIndex].RSSI + abs(INT8_MIN)),&Pans.Pool[panArrIndex]);
		

		panArrIndex++;

	}
	

	
	return panArrIndex;
}

PanPoolType * getPanPool(void){
	return &Pans;
}





uint8_t xbee_pack_remoteAT_frame(AT_commandType *AT_Command, uint8_t * buffer)
{
	uint8_t temp_buffer[SINGLE_FRAME_LENGTH];
	uint8_t index = 0;
	
	uint8_t *data = AT_Command->data;
	
	#ifdef USE_XBEE

	temp_buffer[0] = 0x7E;		// Start delimiter
	// Let index 1 & 2 free for storing frame length
	temp_buffer[3] = 0x17;   	// API Identifier Value for 64-bit TX Request message will cause the module to send RF Data as an RF Packet.
	temp_buffer[4] = 0x50;		// Constant Frame ID arbitrarily selected
	// Identifies the UART data frame for the host to correlate with a subsequent ACK (acknowledgment).
	// Setting Frame ID to �0' will disable response frame.
	
	// 32-bit destination address high
	temp_buffer[5] = (uint8_t)(xbee.dest_high >> 24);
	temp_buffer[6] = (uint8_t)(xbee.dest_high >> 16);
	temp_buffer[7] = (uint8_t)(xbee.dest_high >> 8);
	temp_buffer[8] = (uint8_t) xbee.dest_high;
	
	// 32-bit destination address low
	temp_buffer[9] = (uint8_t) (xbee.dest_low >> 24);
	temp_buffer[10] = (uint8_t)(xbee.dest_low >> 16);
	temp_buffer[11] = (uint8_t)(xbee.dest_low >> 8);
	temp_buffer[12] = (uint8_t) xbee.dest_low;
	
	temp_buffer[13] = 0xFF;				// 16-Bit network Adress
	temp_buffer[14] = 0xFE;     		// default 0xFFFE
	
	temp_buffer[15] = 0;				// Remote command options
	
	temp_buffer[16] = AT_Command->MSC_AT_CODE;
	temp_buffer[17] = AT_Command->LSC_AT_CODE;
	#endif
	

	index = 18;
	

	// Parameter
	while (AT_Command->data_len)
	{
		temp_buffer[index] = *data++;
		index++;
		AT_Command->data_len--;
	}
	
	// Calculate checksum
	temp_buffer[index] = xbee_getChecksum(temp_buffer,3,index);
	
	// Add frame length - excludes Start delimiter, Length and checksum
	temp_buffer[1] = ((index-3) >> 8);
	temp_buffer[2] = (index-3);
	
	memcpy(buffer, temp_buffer, index+1);		// Frame is in the params array
	
	return index+1;

}
