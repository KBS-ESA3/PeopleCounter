/******************************************************************************
 * File           : Main program - I2C 2x16 LCD BTHQ21605V
 *****************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "se24lc512.h"    //serial eeprom
#include "helper.h"
#include "string.h"
#include "usart.h"

#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
I2C_TypeDef I2C_handler;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void I2C_Setup();
void GPIO_Setup();
// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	char buffer[50];
  uint16_t dev = 0x29, wordData;   //arbitrary address;
  uint8_t  status, byteData, sensorState = 0, i;
	
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
	VL53L1X_SensorInit(dev);

  I2C_Setup();
	GPIO_Setup();
	USART_init();
  USART_clearscreen();
  USART_putstr("VL53 test\n\n");
	
	status = VL53L1_RdByte(dev, 0x010F, &byteData);
	sprintf(buffer, "%d\n", byteData);
	USART_putstr(buffer);
	status = VL53L1_RdByte(dev, 0x0110, &byteData);
	sprintf(buffer, "%d\n", byteData);
	USART_putstr(buffer);
	status = VL53L1_RdWord(dev, 0x010F, &wordData);
	sprintf(buffer, "%d\n", byteData);
	USART_putstr(buffer);
	
	while(sensorState==0){
		status = VL53L1X_BootState(dev, &sensorState);
		Delay(2);
  }
  USART_putstr("\nChip Booted\n");
  while(1)
  {
	  
  }
}
//			USART_putstr("You pressed: ");
//      USART_putc(data_from_array);
//      USART_putstr("\n");

void I2C_Setup(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  // Set I2C1 clock to SYSCLK (see system_stm32f0.c)
  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

  //(#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
  //    function for I2C1 or I2C2.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  //(#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
  //    RCC_AHBPeriphClockCmd() function. 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  //(#) Peripherals alternate function: 
  //    (++) Connect the pin to the desired peripherals' Alternate 
  //         Function (AF) using GPIO_PinAFConfig() function.
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

  //    (++) Configure the desired pin in alternate function by:
  //         GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

  //    (++) Select the type, OpenDrain and speed via  
  //         GPIO_PuPd, GPIO_OType and GPIO_Speed members
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  //    (++) Call GPIO_Init() function.
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //(#) Program the Mode, Timing , Own address, Ack and Acknowledged Address 
  //    using the I2C_Init() function.
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
//I2C_InitStructure.I2C_Timing = 0x00310309; // ~400 kHz. @ 8 MHz (HSI) see Ref. Man. Table 91
//I2C_InitStructure.I2C_Timing = 0x50330309; // ~400 kHz. @ 48 MHz (SYSCLK) see Ref. Man. Table 93
//  I2C_InitStructure.I2C_Timing = 0x2033030A; // =400 kHz. @ 48 MHz (SYSCLK) measured with Logic Analyzer
  I2C_InitStructure.I2C_Timing = 0xB0420F13; // =100 kHz. @ 48 MHz (SYSCLK) See Table 93

  I2C_Init(I2C1, &I2C_InitStructure);
  
  //(#) Optionally you can enable/configure the following parameters without
  //    re-initialization (i.e there is no need to call again I2C_Init() function):
  //    (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function.
  //    (++) Enable the dual addressing mode using I2C_DualAddressCmd() function.
  //    (++) Enable the general call using the I2C_GeneralCallCmd() function.
  //    (++) Enable the clock stretching using I2C_StretchClockCmd() function.
  //    (++) Enable the PEC Calculation using I2C_CalculatePEC() function.
  //    (++) For SMBus Mode:
  //         (+++) Enable the SMBusAlert pin using I2C_SMBusAlertCmd() function.

  //(#) Enable the NVIC and the corresponding interrupt using the function
  //    I2C_ITConfig() if you need to use interrupt mode.
  
  //(#) When using the DMA mode 
  //   (++) Configure the DMA using DMA_Init() function.
  //   (++) Active the needed channel Request using I2C_DMACmd() function.
  
  //(#) Enable the I2C using the I2C_Cmd() function.
  I2C_Cmd(I2C1, ENABLE);

  //(#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
  //    transfers.
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
