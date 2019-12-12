/******************************************************************************
 * File           : project configuration for easy switching betweeen boards
 *****************************************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

// ----------------------------------------------------------------------------
// Global definitions
// ----------------------------------------------------------------------------

/*choose board here.
*uncomment the board you want to work with.
*/
#define BOARD_DISCOVERY
//#define BOARD_LORA

//project library's
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "delay.h"


//board specific library's
#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	#include "stm32f0xx.h"	
	#else
	#error more than one board selected in config.h!
	#endif
	#else
	#ifdef BOARD_LORA
	//lora board implementation
	
	#else
	#error no board selected in config.h!
	#endif	
	#endif


#include "hardware_communication.h"
#include "calipile_discovery.h"


#endif /* _CONFIG_H_ */

