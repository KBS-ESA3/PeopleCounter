#ifndef _CALIPILE_DISCOVERY_H_
#define _CALIPILE_DISCOVERY_H_

#include "stdint.h"
// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
//calipile register adresses
#define IR_TPOBJECT 1
#define IR_TPAMBIENT 3
#define IR_TPLOWPASS1 5
#define IR_TPLOWPASS2 7
#define IR_TPLOWPASS3 10
#define IR_TPLOWPASS2FROZEN 12
#define IR_TPPRESENCE 15
#define IR_TPMOTION 16
#define IR_TPAMBSHOCK 17
#define IR_INTERRUPTSTATUS 18
#define IR_CHIPSTATUS 19
#define IR_SETLOWPASS1 20
#define IR_SETLOWPASS2 20
#define IR_SETLOWPASS3 21
#define IR_TPPRESENCETRSHLD 22
#define IR_TPMOTIONTRSHLD 23
#define IR_TPAMBSHCKTRSHLD 24
#define IR_INTERRUPTMASK 25
#define IR_CYCLETIME 26
#define IR_SRCSELECT 26
#define IR_INTERRUPTDIR 26
#define IR_TIMINTERRUPT 27
#define IR_OBJECTTRSHLD 28
#define IR_EEPROMCTRL 31
#define IR_EEPROMREAD 62
#define IR_SLAVEADRESS 63

// Functions
int16_t calipile_get_presence();
uint8_t calipile_read_data(uint8_t register_address);
void calipile_write_data(uint8_t register_address, uint8_t val);
void calipile_general_call(void);
void calipile_init();

#endif /* _CALIPILE_DISCOVERY_H_ */

