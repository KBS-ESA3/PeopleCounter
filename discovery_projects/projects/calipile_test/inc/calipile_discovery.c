/* Includes ------------------------------------------------------------------*/
#include "calipile_discovery.h"

void calipile_generalCall(void)
{
  // Wait while I2C peripheral is not ready
  WaitForI2CFlag( I2C_ISR_BUSY );
  // Start I2C write transfer for 3 bytes
  I2C_TransferHandling( I2C1, 0x00, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  WaitForI2CFlag( I2C_ISR_TXIS );
  // Write eeprom address
  I2C_SendData(I2C1, 0x04);       // Address High byte
  WaitForI2CFlag(I2C_ISR_TC);
  
  // Wait for- and clear stop condition
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

uint8_t calipile_ReadData(Calipile *sensor, uint8_t regAdress)
{
  uint8_t data = 0x0F;

  // Wait while I2C peripheral is not ready
  WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 2 bytes, do not end transfer (SoftEnd_Mode)
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  WaitForI2CFlag(I2C_ISR_TXIS);

  I2C_SendData(I2C1, regAdress);   // Address Low byte
  WaitForI2CFlag(I2C_ISR_TC);
  // Repeated start I2C read transfer for 1 byte
  I2C_TransferHandling(I2C1, sensor->I2CAdress, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
  WaitForI2CFlag(I2C_ISR_RXNE);
  
  // Read data
  data = I2C_ReceiveData(I2C1);
    
  // Wait for- and clear stop condition
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;
  
  return( data );
}



void calipile_writeData(Calipile *sensor, uint8_t reg, uint8_t val)
{
	// Wait while I2C peripheral is not ready
  WaitForI2CFlag( I2C_ISR_BUSY );
  // Start I2C write transfer for 3 bytes
  I2C_TransferHandling( I2C1, sensor->I2CAdress, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );
  WaitForI2CFlag( I2C_ISR_TXIS );
  // Write eeprom address
  I2C_SendData(I2C1, reg);       // Address High byte
	WaitForI2CFlag( I2C_ISR_TXIS );
	I2C_SendData(I2C1, val);       // Address High byte
	WaitForI2CFlag(I2C_ISR_TC);
  
  // Wait for- and clear stop condition
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
	
}

uint32_t calipile_getTPObject(Calipile *sensor)
{
	uint32_t reading = 0;
	reading |= calipile_ReadData(sensor,3);
	reading |= calipile_ReadData(sensor,2)<<8;
	reading |= calipile_ReadData(sensor,1)<<16;
	reading = reading>>7;
	return reading;	
	
}

void calipile_init(Calipile *sensor)
{
	uint8_t data = 0;
	
	//start calipile communication
	calipile_generalCall();
	
	//set low pass filter values
	data |= sensor->Slp1; 
	data |= sensor->Slp2 << 4;
	calipile_writeData(sensor,IR_SETLOWPASS1,data);
	calipile_writeData(sensor,IR_SETLOWPASS2,data);
	calipile_writeData(sensor,IR_SETLOWPASS3,sensor->Slp3);
	data = 0;
	
	//set interrupt treshold values
	calipile_writeData(sensor,IR_TPPRESENCETRSHLD,sensor->presenceTrshld);
	calipile_writeData(sensor,IR_TPMOTIONTRSHLD,sensor->motionTrshld);
	calipile_writeData(sensor,IR_TPAMBSHOCK,sensor->ambShockTrshld);
	calipile_writeData(sensor,IR_OBJECTTRSHLD,sensor->objectTrshld);
	//set interrupt enable bits
	calipile_writeData(sensor,IR_INTERRUPTMASK,sensor->interruptMask);
	//set other detection settings
	calipile_writeData(sensor,IR_CYCLETIME,sensor->motionCycleTime);
	calipile_writeData(sensor,IR_SRCSELECT,sensor->presenceSrcSelect);
	calipile_writeData(sensor,IR_INTERRUPTDIR,sensor->objectIntDir);
	//set timer interrupt value
	calipile_writeData(sensor,IR_TIMINTERRUPT,sensor->timInt);
	calipile_writeData(sensor,IR_OBJECTTRSHLD,(sensor->objectTrshld &0xFF));
	calipile_writeData(sensor,IR_OBJECTTRSHLD+1,(sensor->objectTrshld &0xFF00)>>8);	
}

int16_t calipile_getTPMotion(Calipile *sensor)
{
	int16_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPMOTION);
	if((calipile_ReadData(sensor,IR_CHIPSTATUS)& (1<<6)))
	{
		reading *= -1;
	}
	return reading;
	
}

int16_t calipile_getTPPresence(Calipile *sensor)
{
	int16_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPPRESENCE);
	if((calipile_ReadData(sensor,IR_CHIPSTATUS)& (1<<7)))
	{
		reading *= -1;
	}
	return reading;
	
}

