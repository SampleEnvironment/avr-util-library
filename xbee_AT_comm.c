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

#include "xbee_AT_comm.h"
#include "xbee.h"
#include "xbee_utilities.h"
#include "heap.h"


void (*print_info_AT)(char *, _Bool )  = NULL;


PanPoolType Pans;


char AT_Lut[11] [2] = {
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
	{'S','D'}
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


uint8_t send_AT(AT_commandType *AT_Command){
	uint8_t buffer[SINGLE_FRAME_LENGTH];
	
	buffer[0] = (uint8_t) AT_Command->MSC_AT_CODE;
	buffer[1] = (uint8_t) AT_Command->LSC_AT_CODE;
	
	memcpy(&buffer[2],AT_Command->data,sizeof(uint8_t)*AT_Command->data_len);

	uint8_t temp_bytes_number = xbee_pack_tx_frame(buffer, AT_Command->data_len+2);
	
	uint8_t reply_Id = xbee_send_and_get_reply(buffer, temp_bytes_number,AT_Command->MSG_TYPE, 20);
	
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
			
			case 16: // Pan Descriptor
			memcpy(&AT_Command->pandesc_S2C,(uint8_t *)frameBuffer[reply_Id].data,sizeof(PanDescriptor_S2CType));
			if (frameBuffer[reply_Id].type == AS_MSG_TYPE)
			{
				break;
			}
			case 22: // Pan Descriptor
			memcpy(&AT_Command->pandesc_S1C,(uint8_t *)frameBuffer[reply_Id].data,sizeof(PanDescriptor_S1CType));
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
		double nChannels = countSetBits(xbee.ScanChannels);
		xbee.scanDurarion = (((nChannels * (2 << AT_command.byte) * 16) + (38 * nChannels) + 20) / 1000)+1;
		
		return 1;
	}
	
	return 0;
	
	
}
uint8_t xbee_set_Scan_Duration(uint8_t dur_exp){
	
	AT_commandType AT_command;

	initAt_set(&AT_command,SD_MSG_TYPE,&dur_exp,1);
	
	send_AT(&AT_command);
	
	if (AT_command.AnswerReceived == false)  	// NO Answer from xbee --> not associated
	{
		return 0;
	}
	return 1;
	
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


uint8_t xbee_is_connected(void)
{

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
	
	char print_sl[10];
	
	for (uint8_t i =0;i <4;i++)
	{
		
		sprintf(print_sl,"%i",AT_command.data[i]);
		//print_info_xbee(print_sl,1);
		_delay_ms(5000);
	}

	
	

	#endif // USE_XBEE
	#ifdef USE_LAN

	#endif
	
	return SL_Address;
}

// Get high and low bytes of basis station
// Used only with "DL" and "DH" command type
uint32_t xbee_get_address_block(uint8_t cmd_type)
{
	

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
}

uint8_t xbee_DA_initiate_reassociation(void){
	AT_commandType AT_command;
	
	initAt_read(&AT_command,DA_MSG_TYPE);
	send_AT(&AT_command);

	if(AT_command.AnswerReceived == false) return 0;
	
	return 1;
}

uint8_t xbee_JV_verification(void){
	AT_commandType AT_command;
	
	initAt_read(&AT_command,JV_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	return AT_command.byte;
	
}

uint8_t xbee_hardware_version(void){
	#ifdef USE_XBEE

	AT_commandType AT_command;
	
	initAt_read(&AT_command,HV_MSG_TYPE);
	send_AT(&AT_command);
	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	uint16_t hw_version_16 = AT_command.dataword ;
	
	
	version.hw_version_xbee = (hw_version_16 > 0x2000)? XBEE_V_SC2 : XBEE_V_S1;
	
	#endif // USE_XBEE
	#ifdef USE_LAN
	version.hw_version_xbee = XPORT;
	#endif
	
	return version.hw_version_xbee;
}

uint16_t xbee_Scan_Channels(void){
	AT_commandType AT_command;
	
	initAt_read(&AT_command,SC_MSG_TYPE);
	send_AT(&AT_command);

	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	uint16_t ScanChannels = AT_command.dataword ;
	
	
	return ScanChannels;
	
}

uint8_t xbee_Set_Scan_Channels(uint16_t SC_Bitfield){
	AT_commandType AT_command;
	
	uint8_t buffer[2];
	// xbee uses little endian byte order
	 buffer[0] = SC_Bitfield >> 8 ;
	 buffer[1] = SC_Bitfield ;
	
	initAt_set(&AT_command,SC_MSG_TYPE,buffer,2);
	send_AT(&AT_command);

	
	if(AT_command.AnswerReceived == false) return 0;
	
	
	return 1;
	
}

uint8_t xbee_Active_Scan(void){
	print_info_AT("Active Scan    ",1);
		
	
	
	uint8_t panArr_index = 0;
	uint8_t reply_ID = 0;
	
	AT_commandType AT_command;
	
	Pans.nPans = 0;
	
	if(!xbee_get_Scan_duration()){
		print_info_AT("get SD Failed",1);
		return 0;
	}
	
	
	resetHeap(&Pans.Heap);
		
	initAt_read(&AT_command,AS_MSG_TYPE);
	send_AT(&AT_command);

	if(AT_command.AnswerReceived == false){
		return 0;
	}
	
	panArr_index = addFrameToPanPool(reply_ID,panArr_index);
	
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
}

uint8_t addFrameToPanPool(uint8_t reply_ID,uint8_t panArrIndex){
	
	if (frameBuffer[reply_ID].data_len == 22)
	{
		
		memcpy(&Pans.Pool[panArrIndex].S1C,(uint8_t *)frameBuffer[reply_ID].data,sizeof(PanDescriptor_S1CType));
		Pans.Pool[panArrIndex].HW = XBEE_V_S1;
		
		push(&Pans.Heap,(uint8_t) (Pans.Pool[panArrIndex].S1C.RSSI + abs(INT8_MIN)),&Pans.Pool[panArrIndex]);
		

		panArrIndex++;

	}
	
	if (frameBuffer[reply_ID].data_len == 16)
	{
		
		memcpy(&Pans.Pool[panArrIndex].S2C,(uint8_t *)frameBuffer[reply_ID].data,sizeof(PanDescriptor_S2CType));
		Pans.Pool[panArrIndex].HW = XBEE_V_SC2;
		
		push(&Pans.Heap,(uint8_t) (Pans.Pool[panArrIndex].S2C.RSSI + abs(INT8_MIN)),&Pans.Pool[panArrIndex]);
		

		panArrIndex++;
		
	}
	
	return panArrIndex;
}

PanPoolType * getPanPool(void){
	return &Pans;
}
	