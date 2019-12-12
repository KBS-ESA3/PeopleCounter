/* Includes ------------------------------------------------------------------*/
#include "hardware_communication.h"



// SE_24LC512 communication status information
#define I2C_COMM_OK            (0x0)
#define I2C_COMM_ERROR         (0x1)
uint8_t I2C_CommStatus = I2C_COMM_OK;
/**
  * @brief  initialize the I2C hardware at 400KHz
  * @param  None
  * @retval None
  */
void I2C_Setup(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	
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
	I2C_InitStructure.I2C_Timing = 0x2033030A; // =400 kHz. @ 48 MHz (SYSCLK) measured with Logic Analyzer
  //I2C_InitStructure.I2C_Timing = 0xB0420F13; // =100 kHz. @ 48 MHz (SYSCLK) See Table 93

  I2C_Init(I2C1, &I2C_InitStructure); 
  I2C_Cmd(I2C1, ENABLE);

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
}

/**
  * @brief  send one byte to I2C transmit register
	* @param  data: data to be transmitted to I2C bus
  * @retval None
  */
void I2C_sendData(uint8_t data)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	WaitForI2CFlag( I2C_ISR_TXIS );
  I2C_SendData(I2C1, data);
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
	
}

/**
  * @brief  reads one byte from I2C recieve register
	* @param  None
  * @retval byte recieved from I2C bus
  */
uint8_t I2C_readData(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	WaitForI2CFlag(I2C_ISR_RXNE);
  return I2C_ReceiveData(I2C1);
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
	
}

/**
  * @brief  starts write command to I2C slave
	* @param  adress: the 7 bit I2C slave adress to start write action
	* @param  bytes: number of bytes to send before stop or restart
	* @param  autoStop: automatically end write with stop bit after number of bytes is sent 0 disabled 1 enabled
  * @retval None
  */
void I2C_startWrite(uint8_t adress, uint8_t bytes, uint8_t autoStop)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	WaitForI2CFlag( I2C_ISR_BUSY );
	if(autoStop)
	{
		I2C_TransferHandling( I2C1, adress, bytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Write );	
	}
	else
	{
		I2C_TransferHandling( I2C1, adress, bytes, I2C_SoftEnd_Mode, I2C_Generate_Start_Write );
	}  
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
	
	
}

/**
  * @brief  starts read command from I2C slave
	* @param  adress: the 7 bit I2C slave adress to start read action
	* @param  bytes: number of bytes to read before stop or restart
	* @param  autoStop: automatically end read with stop bit after number of bytes is sent 0 disabled 1 enabled
  * @retval None
  */
void I2C_startRead(uint8_t adress, uint8_t bytes, uint8_t autoStop)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
	WaitForI2CFlag( I2C_ISR_BUSY );
	if(autoStop)
	{
		I2C_TransferHandling( I2C1, adress, bytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Read );	
	}
	else
	{
		I2C_TransferHandling( I2C1, adress, bytes, I2C_SoftEnd_Mode, I2C_Generate_Start_Read );
	}	
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
	
	
}

/**
  * @brief  clears I2C bus stop flag
	* @param  None
  * @retval None
  */
void I2C_stop(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
  WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
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
	
	
}

/**
  * @brief  checks of transfer is done
	* @param  None
  * @retval 1 for complete 0 for timeout
  */
uint8_t I2C_checkTranferComplete(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
  WaitForI2CFlag(I2C_ISR_TC);
	return 1;
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
	
	
}

void USART_init(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
  // GPIOA Periph clock enable
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  // PA9 and PA10 Alternate function mode
  GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
  // Set alternate functions AF1 for PA9 and PA10
  GPIOA->AFR[1] |= 0x00000110;
  // USART1 clock enable
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 
  // 115200 Bd @ 48 MHz
  // USARTDIV = 48 MHz / 115200 = 416 = 0x01A0
  // BRR[15:4] = USARTDIV[15:4]
  // When OVER8 = 0, BRR [3:0] = USARTDIV [3:0]
  USART1->BRR = 0x01A0;
  // USART enable
  // Receiver enable
  // Transmitter enable
  USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;
  // Default value
  USART1->CR2 = 0;
  USART1->CR3 = 0; 
   // RXNE interrupt enable
   USART1->CR1 |= USART_CR1_RXNEIE;
   // USART1 interrupts enable in NVIC
   NVIC_EnableIRQ(USART1_IRQn);
   NVIC_SetPriority(USART1_IRQn, 0);
   NVIC_ClearPendingIRQ(USART1_IRQn);
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
  
	 
}

void USART_putc(char c)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
  // Wait for Transmit data register empty
  while((USART1->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART1->TDR = c;
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
  
}

void USART_putstr(char *str)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
    while(*str)
  {
    if(*str == '\n')
    {
      USART_putc('\r');
    }
    
    USART_putc(*str++);
  }
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

}

void USART_putint(int16_t v)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
    static char str[10];
	
  // first convert an integer to an ASCII string of characters, terminated with a NULL
  // v: the number to convert. Negative numbers are also allowed.
  // p: pointer to the destination buffer
  int16_t t1 = 0, t2 = 0;
  char h[10];

  if (v < 0)
  {
    str[t2] = '-';
    t2++;
    v = -v;
  }
  
  do
  {
    h[t1] = v % 10 + 48;
    t1++;
    v = v / 10;

  } while (v > 0);

  while (t1 > 0)
  {
    str[t2++] = h[--t1];
  }

  str[t2] = '\0';
  USART_putstr(str);
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

}

void USART_clearscreen(void)
{
	#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  USART_putstr(cmd1);
  USART_putstr(cmd2);
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
  
}
// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------
#ifdef BOARD_DISCOVERY
	#ifndef BOARD_LORA
	//discovery board implementation

#define I2C_TIMEOUT ((uint32_t)(0x00001000))
		void WaitForI2CFlag(uint32_t flag)
{
  uint32_t timeout = I2C_TIMEOUT;
  
  if(flag == I2C_ISR_BUSY)
  {
    while(I2C_GetFlagStatus(I2C1, flag) != RESET)
    {
      if(timeout-- == 0)
      {
        I2C_CommStatus = I2C_COMM_ERROR;
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
        I2C_CommStatus = I2C_COMM_ERROR;
        return;
      }
    }
  }
}
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


