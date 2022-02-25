/*
 * xbee_AT_comm.h
 *
 * Created: 16.02.2022 09:39:52
 *  Author: qfj
 */ 


#ifndef XBEE_AT_COMM_H_
#define XBEE_AT_COMM_H_

#include "heap.h"


typedef struct
{
	uint8_t  AS_type;  //  should be ==2 for zigbee modules
	uint8_t  Channel_ID;
	uint16_t PAN;
	uint64_t ExtendedPAN;
	uint8_t  AllowJoin;
	uint8_t  StackProfile;
	uint8_t  LinkQualityIndicator;
	int8_t   RSSI;

}PanDescriptor_S2CType;

#define PAN_POOL_SIZE 10

typedef struct  
{
	PanDescriptor_S2CType Pool[PAN_POOL_SIZE];
	uint8_t AlreadyTried[PAN_POOL_SIZE];
	HeapType Heap;
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
		PanDescriptor_S2CType pandesc;
	};
	uint8_t data_len;
	_Bool AnswerReceived;
	
	
}AT_commandType;



#define  AT_START 18

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
}AT_MESSAGE; 




AT_commandType* initAt_set(AT_commandType * Atcommand,AT_MESSAGE AT_Code,uint8_t * data,uint8_t data_len);
AT_commandType* initAt_read(AT_commandType * Atcommand,AT_MESSAGE AT_Code);
uint8_t send_AT(AT_commandType *AT_Command);

uint8_t xbee_is_connected(void);
uint32_t xbee_SL_address(void);
uint32_t xbee_get_address_block(AT_MESSAGE AT_Code);
uint8_t xbee_DA_initiate_reassociation(void);
uint8_t xbee_JV_verification(void);
uint16_t xbee_Scan_Channels(void);
uint8_t xbee_Active_Scan(void);
uint8_t xbee_hardware_version(void);




#endif /* XBEE_AT_COMM_H_ */

