/*
 * module_globals.h
 *
 * Created: 13.07.2021 13:23:46
 *  Author: qfj
 */ 


#ifndef MODULE_GLOBALS_H_
#define MODULE_GLOBALS_H_

#include <stdint.h>

//====================================================================
//			VERSION
//====================================================================
typedef struct{
	// Version vars
	uint16_t Fw_version;
	uint8_t Branch_id;
	uint16_t FW_eeprom_changed ;
	uint8_t hw_version_xbee ;

}VersionType;


extern volatile uint32_t  count_t_elapsed;







#endif /* MODULE_TYPEDEFS_H_ */