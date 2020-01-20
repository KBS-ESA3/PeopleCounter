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
	calipile_general_call();

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

