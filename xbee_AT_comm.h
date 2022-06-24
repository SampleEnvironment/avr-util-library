/*
* xbee_AT_comm.h
*
* Created: 16.02.2022 09:39:52
*  Author: qfj
*/


#ifndef XBEE_AT_COMM_H_
#define XBEE_AT_COMM_H_

#include "heap.h"
#include "xbee.h"



typedef struct
{
	uint64_t CoordSerialNumber;
	uint16_t PanID;
	uint8_t  CoordAddrMode;
	uint8_t  ChannelID;
	uint8_t  SecurityUse;
	uint8_t  ACLEntry;
	uint8_t SecurityFailure;
	uint16_t SuperFrameSpec;
	uint8_t  GtsPermin;
	int8_t   RSSI;
	uint8_t  Timestamp[3];
}PanDescriptor_15_4;

typedef struct
{
	uint64_t CoordSerialNumber;
	uint16_t PanID;
	uint8_t  ChannelID;
	int8_t   RSSI;
}PandescShortType;


#define PAN_POOL_SIZE 10

typedef struct
{
	PandescShortType Pool[PAN_POOL_SIZE];
	uint8_t AlreadyTried[PAN_POOL_SIZE];
	HeapType Heap;
	uint8_t nPans;
}PanPoolType;

typedef struct
{
	uint8_t MSG_TYPE;
	
	char MSC_AT_CODE;
	char LSC_AT_CODE;
	
	union
	{
		uint8_t byte;
		uint16_t dataword;
		uint32_t Address;
		uint8_t data[25];
		PanDescriptor_15_4 pandesc;
	};
	uint8_t data_len;
	_Bool AnswerReceived;
	
	
}AT_commandType;

extern XbeeType xbee;


#define  AT_START 200

typedef enum
{
	DA_MSG_TYPE 	= AT_START,		// Force dissociation API command
	DH_MSG_TYPE, 					// Destination address high API command
	DL_MSG_TYPE, 					// Destination address low API command
	STATUS_MSG_TYPE, 				//
	AI_MSG_TYPE,				    // Association Indication Reads errors with the last association request.
	HV_MSG_TYPE,				    // Hardvare version of xbee module
	SL_MSG_TYPE,				    // xbeeaddress of module
	AS_MSG_TYPE,				    // Module performs an sctive Scan and retrns PAN descriptors of all Coordinators within range
	SC_MSG_TYPE,                    // Scan channels reads the list of channels to scan
	JV_MSG_TYPE,				    // Coordinator join verification
	SD_MSG_TYPE,					// Scan Duration
	VR_MSG_TYPE,					// Reads the xbee Firmware version
	WR_MSG_TYPE,					// writes changes to xbee flash
	NI_MSG_TYPE,					// reads node identifier
	CE_MSG_TYPE,					// coordinator enable bit
	SM_MSG_TYPE,					// Sleep mode
	CH_MSG_TYPE,					// operating Channel
	ZS_MSG_TYPE,					// Stack profile
	NJ_MSG_TYPE,					// Node Join Time
	A1_MSG_TYPE,					// coordinator behavior
	A2_MSG_TYPE,					// end dev behaviour
	DB_MSG_TYPE						// RSSI of last received message
}AT_MESSAGE;




AT_commandType* initAt_set(AT_commandType * Atcommand,AT_MESSAGE AT_Code,uint8_t * data,uint8_t data_len);
AT_commandType* initAt_read(AT_commandType * Atcommand,AT_MESSAGE AT_Code);
void send_AT_noRep(AT_commandType *AT_Command);
uint8_t send_AT(AT_commandType *AT_Command);
uint8_t send_remoteAT(AT_commandType *AT_Command);

uint8_t AT_read_Byte( AT_MESSAGE AT_Code);
uint8_t AT_set_Byte( AT_MESSAGE AT_Code,uint8_t param);
uint8_t xbee_is_connected(void);
uint32_t xbee_SL_address(void);
uint32_t xbee_get_address_block(AT_MESSAGE AT_Code);
uint8_t xbee_DA_initiate_reassociation(void);
uint8_t xbee_JV_verification(void);
uint8_t xbee_Node_Join(void);
uint8_t xbee_set_Node_Join(uint8_t NJ);
uint8_t xbee_stack_profile(void);
uint8_t xbee_sleep_Mode(void);
uint8_t xbee_set_sleep_Mode(uint8_t SM);
uint8_t xbee_coordinator_Enable(void);
uint8_t xbee_set_coordinator_Enable(uint8_t CE);
uint8_t xbee_Assiciation_indication(void);
uint16_t xbee_Scan_Channels(void);
uint8_t xbee_get_DB(void);
uint8_t xbee_Set_Scan_Channels(uint16_t SC_Bitfield);
uint8_t xbee_clear_Curr_Channel_from_SC(void);
uint8_t xbee_Active_Scan(void);
uint8_t xbee_WR(void);
uint16_t xbee_hardware_version(void);
uint8_t xbee_channel(void);
uint16_t xbee_firmware_version(void);
uint8_t addFrameToPanPool(uint8_t reply_ID,uint8_t panArrIndex);
uint32_t countSetBits(uint32_t n);
PanPoolType * getPanPool(void);
uint8_t xbee_pack_remoteAT_frame(AT_commandType *AT_Command, uint8_t * buffer);
uint8_t xbee_coordIdentifier(void);



#endif /* XBEE_AT_COMM_H_ */

