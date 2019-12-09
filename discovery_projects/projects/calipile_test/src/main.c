/******************************************************************************
 * File           : Main program - I2C 2x16 LCD BTHQ21605V
 *****************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"

#define DELAY_MS SystemCoreClock/8/1000
#define DELAY_S SystemCoreClock/8

//the standard peripheral library cuts the first bit from the adress for read/write bit so shift it 1 bit to the right
#define CALIPILE_0_ADDR (0x0C<<1)
#define CALIPILE_1_ADDR (0x0D<<1)

//calipile register adresses
#define IR_TPOBJECT 1
#define IR_TPAMBIENT 3
#define IR_TPLOWPASS1 5
#define IR_TPLOWPASS2 7
#define IR_TPLOWPASS3 10
#define IR_TPLOWPASS2FROZEN 12
#define IR_TPPRESENCE 15
#define IR_TPMOTION 16
#define IR_TPAMBSHOCK 17
#define IR_INTERRUPTSTATUS 18
#define IR_CHIPSTATUS 19
#define IR_SETLOWPASS1 20
#define IR_SETLOWPASS2 20
#define IR_SETLOWPASS3 21
#define IR_TPPRESENCETRSHLD 22
#define IR_TPMOTIONTRSHLD 23
#define IR_TPAMBSHCKTRSHLD 24
#define IR_INTERRUPTMASK 25
#define IR_CYCLETIME 26
#define IR_SRCSELECT 26
#define IR_INTERRUPTDIR 26
#define IR_TIMINTERRUPT 27
#define IR_OBJECTTRSHLD 28
#define IR_EEPROMCTRL 31
#define IR_EEPROMREAD 62
#define IR_SLAVEADRESS 63


// Maximum timeout value for waiting loops
// This timeout is not based on an accurate timing value. It just guarantess
// that the program will not get stuck in an endless loop whenever there is an
// error in the I2C communication
#define I2C_TIMEOUT ((uint32_t)(0x00001000))

// SE_24LC512 communication status information
#define I2C_COMM_OK            (0x0)
#define I2C_COMM_ERROR         (0x1)

struct Calipile{
	uint8_t I2CAdress, 
	Slp1, 
	Slp2, 
	Slp3, 
	presenceTrshld, 
	motionTrshld, 
	ambShockTrshld,	
	interruptMask, 
	motionCycleTime, 
	presenceSrcSelect,
	objectIntDir,
	timInt;
	uint16_t objectTrshld;
};

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint8_t I2C_CommStatus = I2C_COMM_OK;
struct Calipile sensor0 = {CALIPILE_0_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};
struct Calipile sensor1 = {CALIPILE_1_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};





// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void USART_init(void);
void USART_putc(char c);
void USART_putstr(char *str);
void USART_putint(int16_t v);
void USART_clearscreen(void);

void I2C_Setup(void);
void WaitForI2CFlag(uint32_t flag);
void calipile_generalCall(void);
uint8_t calipile_ReadData(struct Calipile *sensor, uint8_t regAdress);
void calipile_writeData(struct Calipile *sensor, uint8_t reg, uint8_t val);
uint32_t calipile_getTPObject(struct Calipile *sensor);
void calipile_init(struct Calipile *sensor);
int16_t calipile_getTPMotion(struct Calipile *sensor);
int16_t calipile_getTPPresence(struct Calipile *sensor);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	char buffer[50];
	uint8_t data = 0;
	uint32_t thermopile = 0;
	int16_t motion0 = 0;
	int16_t presence0 = 0;
	int16_t motion1 = 0;
	int16_t presence1 = 0;
  USART_init();
	USART_putstr("Motion0 presence0 Motion1 presence1\n");
	I2C_Setup();
	calipile_init(&sensor0);
	calipile_init(&sensor1);
	
  while(1)
  {
	  thermopile = calipile_getTPObject(&sensor0);
		motion0 = calipile_getTPMotion(&sensor0);
		presence0 = calipile_getTPPresence(&sensor0);
		motion1 = calipile_getTPMotion(&sensor1);
		presence1 = calipile_getTPPresence(&sensor1);
		sprintf(buffer,"%d %d %d %d\n",motion0,presence0,motion1,presence1);
		USART_putstr(buffer);
		Delay(DELAY_MS*10);
  }
}


void USART_init(void)
{
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
	 
}

void USART_putc(char c)
{
  // Wait for Transmit data register empty
  while((USART1->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART1->TDR = c;
}

void USART_putstr(char *str)
{
  while(*str)
  {
    if(*str == '\n')
    {
      USART_putc('\r');
    }
    
    USART_putc(*str++);
  }
}

void USART_putint(int16_t v)
{
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
}

void USART_clearscreen(void)
{
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  USART_putstr(cmd1);
  USART_putstr(cmd2);
}

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
	I2C_InitStructure.I2C_Timing = 0x2033030A; // =400 kHz. @ 48 MHz (SYSCLK) measured with Logic Analyzer
  //I2C_InitStructure.I2C_Timing = 0xB0420F13; // =100 kHz. @ 48 MHz (SYSCLK) See Table 93

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
	//I2C_AcknowledgeConfig(I2C1,ENABLE);
	//I2C_GeneralCallCmd(I2C1,ENABLE);
	//I2C_StretchClockCmd(I2C1,ENABLE);

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

uint8_t calipile_ReadData(struct Calipile *sensor, uint8_t regAdress)
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



void calipile_writeData(struct Calipile *sensor, uint8_t reg, uint8_t val)
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
	Delay(DELAY_MS);
	
}

uint32_t calipile_getTPObject(struct Calipile *sensor)
{
	uint32_t reading = 0;
	reading |= calipile_ReadData(sensor,3);
	reading |= calipile_ReadData(sensor,2)<<8;
	reading |= calipile_ReadData(sensor,1)<<16;
	reading = reading>>7;
	return reading;	
	
}

void calipile_init(struct Calipile *sensor)
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

int16_t calipile_getTPMotion(struct Calipile *sensor)
{
	int16_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPMOTION);
	if((calipile_ReadData(sensor,IR_CHIPSTATUS)& (1<<6)))
	{
		reading *= -1;
	}
	return reading;
	
}

int16_t calipile_getTPPresence(struct Calipile *sensor)
{
	int16_t reading = 0;
	reading |= calipile_ReadData(sensor,IR_TPPRESENCE);
	if((calipile_ReadData(sensor,IR_CHIPSTATUS)& (1<<7)))
	{
		reading *= -1;
	}
	return reading;
	
}

