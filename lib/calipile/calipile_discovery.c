/* Includes ------------------------------------------------------------------*/
#include "calipile_discovery.h"

/**
  * @brief  sends a general call to sensors and follows with the startup command.
  * @param  None.
  * @retval None
  */
void calipile_generalCall(void)
{
	const uint8_t command = 0x04;
	I2C_Write(0x00,&command,1);
  
}

/**
  * @brief  reads a register from the sensor only one byte is read.
	* @param  sensor: pointer to struct containing sensor settings.
	* @param  regAdress: adress of the register to read from.
  * @retval the value from the sensor register
  */
uint8_t calipile_ReadData(Calipile *sensor, uint8_t regAdress)
{
  	uint8_t data = 0x00;
	I2C_ReadRegister8(sensor->I2CAdress,regAdress,&data,1);  
  	return( data );
}


/**
  * @brief  writes a byte into a specified sensor register adress.
	* @param  sensor: pointer to struct containing sensor settings.
	* @param  reg: adress of the register to write to.
	* @param  val: value to write to the sensor register.
  * @retval None
  */
void calipile_writeData(Calipile *sensor, uint8_t reg, uint8_t val)
{
	I2C_WriteRegister8(sensor->I2CAdress,reg,&val,1);
}

/**
  * @brief  retrieves the raw thermopile adc value.
  * @param  sensor: pointer to struct containing sensor settings.
  * @retval 17 bit value of raw thermopile data
  */
uint32_t calipile_getTPObject(Calipile *sensor)
{
	uint32_t reading = 0;
	reading = ( (uint32_t) ( (uint32_t)calipile_ReadData(sensor,IR_TPOBJECT) << 16) | ( (uint32_t)calipile_ReadData(sensor,IR_TPOBJECT+1) << 8) | ( (uint32_t)calipile_ReadData(sensor,IR_TPOBJECT+2) & 0x80)) >> 7;
	return reading;	
	
	
}

/**
  * @brief  intializes all the sensor registers with the predefined settings in sensor struct.
  * @param  sensor: pointer to struct containing sensor settings.
  * @retval None
  */
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

/**
  * @brief  returns the motion value from sensor register.
  * @param 	sensor: pointer to struct containing sensor settings.
  * @retval 9 bit signed motion value
  */
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

/**
  * @brief  returns the presence value from sensor register.
  * @param 	sensor: pointer to struct containing sensor settings.
  * @retval 9 bit signed presence value
  */
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

/**
  * @brief  returns the presence value from sensor register.
  * @param 	sensor: pointer to struct containing sensor settings.
  * @retval 9 bit signed presence value
  */
uint32_t calipile_getLP1(Calipile *sensor)
{
	uint32_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPLOWPASS1)<<16;
	reading |= calipile_ReadData(sensor,IR_TPLOWPASS1+1)<<8;
	reading |= (calipile_ReadData(sensor,IR_TPLOWPASS1)& 0xF0);
	reading >>= 4;
	
	return reading;
	
}


