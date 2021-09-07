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

extern VersionType version;

void version_INIT(uint16_t Fw_version,uint8_t Branch_id,uint16_t FW_eeprom_changed);

//################## PORT DEFINITIONS PER DEVICE ##################

#ifdef LEVELMETER
#define SDA PINC4
#define SCL PINC2
#endif  


#ifdef GASCOUNTER_MODULE
#define SDA PINC1
#define SCL PINC0
#endif  


#ifdef ILM_MODULE
#define SDA PINC1
#define SCL PINC0
#endif




#endif /* MODULE_TYPEDEFS_H_ */