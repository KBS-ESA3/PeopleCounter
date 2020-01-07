
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

#include "VL53L1X_api.h"
#include "vl53l1_platform.h"
#include "vl53l1_error_codes.h"
#include <string.h>
#include <time.h>
#include <math.h>

#define I2C_TIME_OUT_BASE   10
#define I2C_TIME_OUT_BYTE   1

#ifdef VL53L1_LOG_ENABLE
#define trace_print(level, ...) VL53L1_trace_print_module_function(VL53L1_TRACE_MODULE_PLATFORM, level, VL53L1_TRACE_FUNCTION_NONE, ##__VA_ARGS__)
#define trace_i2c(...) VL53L1_trace_print_module_function(VL53L1_TRACE_MODULE_NONE, VL53L1_TRACE_LEVEL_NONE, VL53L1_TRACE_FUNCTION_I2C, ##__VA_ARGS__)
#endif

#ifndef HAL_I2C_MODULE_ENABLED
#warning "HAL I2C module must be enable "
#endif

//#define VL53L0X_pI2cHandle    (&hi2c1)

/* when not customized by application define dummy one */
#ifndef VL53L1_GetI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L1_GetI2cBus(...) (void)0
#endif

#ifndef VL53L1_PutI2cBus
/** This macro can be overloaded by user to enforce i2c sharing in RTOS context
 */
#   define VL53L1_PutI2cBus(...) (void)0
#endif


int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	I2C_WriteRegister16(dev,index,pdata,count);
	return 0; // to be implemented
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	I2C_ReadRegister16(dev,index,pdata,count);
	return 0; // to be implemented
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	I2C_WriteRegister16(dev,index,&data,1);
	return 0; // to be implemented
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	I2C_WriteRegister16(dev,index,&data,2);
	return 0; // to be implemented
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	I2C_WriteRegister16(dev,index,&data,4);
	return 0; // to be implemented
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	I2C_ReadRegister16(dev,index,&data,1);
	return 0; // to be implemented
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	I2C_ReadRegister16(dev,index,&data,2);
	return 0; // to be implemented
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	I2C_ReadRegister16(dev,index,&data,4);
	return 0; // to be implemented
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	HAL_Delay(wait_ms);
	return 0; // to be implemented
}
