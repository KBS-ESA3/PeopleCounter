/* Includes ------------------------------------------------------------------*/
#include "calipile_discovery.h"

#define SENSOR_ADDRESS CALIPILE_1_ADDR
// Two CaliPile Sensor's were used during testing
#define CALIPILE_0_ADDR (0x0C << 1)
#define CALIPILE_1_ADDR (0x0D << 1)

// Register addresses
#define CALIPILE_LOWPASS1 11
#define CALIPILE_LOWPASS2 8
#define CALIPILE_LOWPASS3 10
#define CALIPILE_PRTRSHLD 50
#define CALIPILE_MTNTRSHLD 10
#define CALIPILE_AMBTRSHLD 10
#define CALIPILE_INTERRUPTMASK 0X00
#define CALIPILE_CYCLETIME 0X08
#define CALIPILE_SOURCE 0X01
#define CALIPILE_INTDIR 0X00
#define CALIPILE_INTTIM 0
#define CALIPILE_OBJTRSHLD 0
#define MIN_PRESENCE 100

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
uint8_t calipile_ReadData(calipile_t *sensor, uint8_t regAdress)
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
void calipile_writeData(calipile_t *sensor, uint8_t reg, uint8_t val)
{
	I2C_WriteRegister8(sensor->I2CAdress,reg,&val,1);
}

/**
  * @brief  retrieves the raw thermopile adc value.
  * @param  sensor: pointer to struct containing sensor settings.
  * @retval 17 bit value of raw thermopile data
  */
uint32_t calipile_getTPObject(calipile_t *sensor)
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
void calipile_Init(calipile_t *sensor)
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
int16_t calipile_getTPMotion(calipile_t *sensor)
{
	int16_t reading = 0;
	reading = calipile_ReadData(sensor,IR_TPMOTION);
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
int16_t calipile_getTPPresence(calipile_t *sensor)
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
int16_t calipile_getTPAmbshock(calipile_t *sensor)
{
	int16_t reading = 0;
	reading |= calipile_ReadData(sensor,17);
	if((calipile_ReadData(sensor,IR_CHIPSTATUS)& (1<<5)))
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
uint32_t calipile_getLP1(calipile_t *sensor)
{
	uint32_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPLOWPASS1)<<16;
	reading |= calipile_ReadData(sensor,IR_TPLOWPASS1+1)<<8;
	reading |= (calipile_ReadData(sensor,IR_TPLOWPASS1)& 0xF0);
	reading >>= 4;
	
	return reading;
	
}
//////////////////////////////////////////////////////////////////////////////
// From here on was rewritten to support just one CaliPile Sensor


// Function reads presence register from CaliPile sensor
int16_t calipile_get_presence()
{
	int16_t reading = 0;
	reading |= calipile_read_data(IR_TPPRESENCE);
	if((calipile_read_data(IR_CHIPSTATUS)& (1<<7)))
	{
		reading *= -1;
	}
	return reading;
}

// Function reads from registers of Calipile sensor
uint8_t calipile_read_data(uint8_t register_address)
{
  	uint8_t data = 0x00;
	I2C_ReadRegister8(SENSOR_ADDRESS, register_address, &data, 1);  
  	return data;
}

// Function writes to registers of CaliPile sensor
void calipile_write_data(uint8_t register_address, uint8_t val)
{
	I2C_WriteRegister8(SENSOR_ADDRESS, register_address, &val,1);
}

// Function to wake up sensor initially
void calipile_general_call(void){
	const uint8_t command = 0x04;
	I2C_Write(0x00,&command,1);
}

// Function sets CaliPile to measure presence
void calipile_init()
{
	uint8_t data = 0;
	// General Call to wake up sensor
	calipile_generalCall();

	//set low pass filter values
	data |= CALIPILE_LOWPASS1;
	data |= CALIPILE_LOWPASS2<< 4;
	calipile_write_data(IR_SETLOWPASS1, data);
	calipile_write_data(IR_SETLOWPASS2, data);
	calipile_write_data(IR_SETLOWPASS3, CALIPILE_LOWPASS3);
	data = 0;
	
	// Set interrupt treshold values
	calipile_write_data(IR_TPPRESENCETRSHLD, CALIPILE_PRTRSHLD);
	calipile_write_data(IR_TPMOTIONTRSHLD, CALIPILE_MTNTRSHLD);
	calipile_write_data(IR_TPAMBSHOCK, CALIPILE_AMBTRSHLD);
	calipile_write_data(IR_OBJECTTRSHLD, CALIPILE_OBJTRSHLD);

	// Set interrupt mask
	calipile_write_data(IR_INTERRUPTMASK,CALIPILE_INTERRUPTMASK);

	calipile_write_data(IR_CYCLETIME, CALIPILE_CYCLETIME);
	calipile_write_data(IR_SRCSELECT, CALIPILE_SOURCE);
	calipile_write_data(IR_INTERRUPTDIR, CALIPILE_INTDIR);

	calipile_write_data(IR_TIMINTERRUPT, CALIPILE_INTTIM);
	calipile_write_data(IR_OBJECTTRSHLD,(CALIPILE_OBJTRSHLD &0xFF));
	calipile_write_data(IR_OBJECTTRSHLD+1,(CALIPILE_OBJTRSHLD &0xFF00)>>8);	
}

