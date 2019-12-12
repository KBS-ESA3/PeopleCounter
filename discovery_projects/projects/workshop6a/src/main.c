/*--- People Counter / VL53L1X test ---*/
#define debug

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"
#include "string.h"
#include "usart.h"
#include "config.h"

#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

#define VL53_I2C_Address 0x29<<1


void GPIO_Setup();
// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	char buffer[50];
  uint16_t wordData, Distance, AmbientRate, SpadNum, SignalRate;
  uint8_t  status, byteData, sensorState = 0, i, dataReady,RangeStatus, ToFSensor = 1;
	
  I2C_Setup();
	GPIO_Setup();
	USART_init();
  USART_clearscreen();
  USART_putstr("VL53 test\n\n");
	#ifdef debug
	status = VL53L1_RdByte(VL53_I2C_Address, VL53L1_IDENTIFICATION__MODEL_ID, &byteData);
	sprintf(buffer, "%x\n", byteData);
	USART_putstr(buffer);
	USART_putc(byteData);
	status = VL53L1_RdByte(VL53_I2C_Address, 0x0110, &byteData);
	sprintf(buffer, "%x\n", byteData);
	USART_putstr(buffer);
	status = VL53L1_RdWord(VL53_I2C_Address, VL53L1_IDENTIFICATION__MODEL_ID, &wordData);
	sprintf(buffer, "%x\n", byteData);
	USART_putstr(buffer);
	status = VL53L1_RdByte(VL53_I2C_Address, 0x0001, &byteData);
	sprintf(buffer, "%x\n", byteData);
	USART_putstr(buffer);
	#endif
	
	while(sensorState==0){
		status = VL53L1X_BootState(VL53_I2C_Address, &sensorState);
		Delay(2);
  }
  USART_putstr("\nChip Booted\n");
	
	status = VL53L1X_SensorInit(VL53_I2C_Address);
	/* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
  status = VL53L1X_SetDistanceMode(VL53_I2C_Address, 2); /* 1=short, 2=long */
  status = VL53L1X_SetTimingBudgetInMs(VL53_I2C_Address, 100); /* in ms possible values [20, 50, 100, 200, 500] */
  status = VL53L1X_SetInterMeasurementInMs(VL53_I2C_Address, 100); /* in ms, IM must be > = TB */
//status = VL53L1X_SetOffset(VL53_I2C_Address,20); /* offset compensation in mm */
//status = VL53L1X_SetROI(VL53_I2C_Address, 16, 16); /* minimum ROI 4,4 */
//status = VL53L1X_CalibrateOffset(VL53_I2C_Address, 140, &offset); /* may take few second to perform the offset cal*/
//status = VL53L1X_CalibrateXtalk(VL53_I2C_Address, 1000, &xtalk); /* may take few second to perform the xtalk cal */
	
	USART_putstr("VL53L1X Ultra Lite Driver Example running ...\n");
  status = VL53L1X_StartRanging(VL53_I2C_Address);   /* This function has to be called to enable the ranging */
  
  while(1)
  {
	  while (dataReady == 0){
		  status = VL53L1X_CheckForDataReady(VL53_I2C_Address, &dataReady);
		  Delay(2);
	  }
		dataReady = 0;
		status = VL53L1X_GetRangeStatus(VL53_I2C_Address, &RangeStatus);
	  status = VL53L1X_GetDistance(VL53_I2C_Address, &Distance);
	  status = VL53L1X_GetSignalRate(VL53_I2C_Address, &SignalRate);
	  status = VL53L1X_GetAmbientRate(VL53_I2C_Address, &AmbientRate);
	  status = VL53L1X_GetSpadNb(VL53_I2C_Address, &SpadNum);
	  status = VL53L1X_ClearInterrupt(VL53_I2C_Address); /* clear interrupt has to be called to enable next interrupt*/
		sprintf(buffer, "%u, %u, %u, %u, %u\n", RangeStatus, Distance, SignalRate, AmbientRate, SpadNum);
		USART_putstr(buffer);
  }
}


void GPIO_Setup()
{
	// GPIOC Periph clock enable
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN; 
  // PC8 and PC9 in output mode
  GPIOC->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0) ;
  // Push pull mode selected
  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9) ;
  // Maximum speed setting (even though it is unnecessary)
  GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);
  // Pull-up and pull-down resistors disabled
  GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);
}
