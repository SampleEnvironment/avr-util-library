/*
 * module_globals.c
 *
 * Created: 13.07.2021 13:29:22
 *  Author: qfj
 */ 

#include "module_globals.h"
#include "../config.h"


VersionType version = {
	.Fw_version = 0,
	.Branch_id = 0,
	.FW_eeprom_changed = 0,
	.hw_version_xbee = 0
};


volatile uint32_t  count_t_elapsed;

void version_INIT(uint16_t Fw_version,uint8_t Branch_id,uint16_t FW_eeprom_changed){
	version.Branch_id = Branch_id;
	version.FW_eeprom_changed = FW_eeprom_changed;
	version.Fw_version = Fw_version;

}