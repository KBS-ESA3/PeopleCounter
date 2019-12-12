#include "stm32f0xx_i2c.h"
#include "VL53L1X_api.h"
#include "vl53l1_platform.h"
#include "vl53l1_error_codes.h"
#include "config.h"
#define I2C_TIME_OUT_BASE   10
#define I2C_TIME_OUT_BYTE   1

uint32_t VL53_CommStatus = VL53_COMM_OK;

VL53L1_Error VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	return Status; 
}

VL53L1_Error VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	return Status;
}

VL53L1_Error VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	I2C_startWrite(dev, 3, 0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_sendData(data);
	I2C_checkTranferComplete();
	I2C_stop();
	
	return Status;
}

VL53L1_Error VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	I2C_startWrite(dev, 6, 0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_sendData(data >> 8);
	I2C_sendData(data & 0xFF);
	I2C_checkTranferComplete();
	I2C_stop();
	
  return Status;
}

VL53L1_Error VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	I2C_startWrite(dev, 6, 0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_sendData((data >> 24) & 0xFF);
	I2C_sendData((data >> 16) & 0xFF);
	I2C_sendData((data >> 8) & 0xFF);
	I2C_sendData((data >> 0) & 0xFF);
	I2C_checkTranferComplete();
	I2C_stop();
	
	return Status;
}

VL53L1_Error VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	I2C_startWrite(dev ,2 ,0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_startRead(dev, 1, 1);
	*data = I2C_readData();
	I2C_checkTranferComplete();
	I2C_stop();
	
	return Status;
}

VL53L1_Error VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;

	I2C_startWrite(dev ,2 ,0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_startRead(dev, 2, 1);
	*data = ((uint16_t)I2C_readData() << 8) + 
					((uint16_t)I2C_readData());
	I2C_checkTranferComplete();
	I2C_stop();
	
	return Status;
}

VL53L1_Error VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	
	I2C_startWrite(dev ,2 ,0);
	I2C_sendData(index >> 8);
	I2C_sendData(index & 0xFF);
	I2C_startRead(dev, 4, 1);
	*data = ((uint32_t)I2C_readData() << 24) +
					((uint32_t)I2C_readData() << 16) +
					((uint32_t)I2C_readData() << 8)  +
					((uint32_t)I2C_readData);
	
	return Status;
}


// Not Implemented
VL53L1_Error VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	return Status;
}

void VL53_WaitForI2CFlag(uint32_t flag)
{
  uint32_t timeout = VL53_TIMEOUT;
  if(flag == I2C_ISR_BUSY)
  {
    while(I2C_GetFlagStatus(I2C1, flag) != RESET)
    {
      if(timeout-- == 0)
      {
        VL53_CommStatus = VL53_COMM_ERROR;
        return;
      }
    }
  }
  else
  {
    while(I2C_GetFlagStatus(I2C1, flag) == RESET)
    {
      if(timeout-- == 0)
      {
        VL53_CommStatus = VL53_COMM_ERROR;
        return;
      }
    }
  }
}
