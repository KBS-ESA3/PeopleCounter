/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

#include "stm32f0xx.h"

// Maximum timeout value for waiting loops
// This timeout is not based on an accurate timing value. It just guarantess
// that the program will not get stuck in an endless loop whenever there is an
// error in the I2C communication
#define I2C_TIMEOUT ((uint32_t)(0x00001000))

// SE_24LC512 communication status information
#define I2C_COMM_OK            (0x0)
#define I2C_COMM_ERROR         (0x1)



// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void  Delay(const int d);
void I2C_Setup(void);
void WaitForI2CFlag(uint32_t flag);

#endif /* _HELPER_H_ */
