/******************************************************************************
 * File           : Main program - I2C 2x16 LCD BTHQ21605V
 *****************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"
#include "calipile_discovery.h"
//#include "VL53L1X_discovery.h"

#define DELAY_MS SystemCoreClock/8/1000
#define DELAY_S SystemCoreClock/8
#define CALIPILE_0_ADDR (0x0C<<1)
#define CALIPILE_1_ADDR (0x0D<<1)



// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
Calipile sensor0 = {CALIPILE_0_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};
Calipile sensor1 = {CALIPILE_1_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};




// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void USART_init(void);
void USART_putc(char c);
void USART_putstr(char *str);
void USART_putint(int16_t v);
void USART_clearscreen(void);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	char buffer[100];
	uint32_t object0 = 0;
	float object = 0.0;
	int16_t motion0 = 0;
	int16_t presence0 = 0;
	uint32_t LP1_0 = 0;
	uint32_t LP2_0 = 0;
	uint32_t LP2FR_0 = 0;
	uint8_t presence = 0;
	uint8_t motion = 0;
  USART_init();
	USART_putstr("Object Motion presence LP1 LP2 LP2FR presence motion\n");
	I2C_Setup();
	calipile_init(&sensor1);
	
  while(1)
  {
		object0 = calipile_getTPObject(&sensor1);
		object = (float)object0/512.0;
		motion0 = calipile_getTPMotion(&sensor1);
		presence0 = calipile_getTPPresence(&sensor1);
		LP1_0 = calipile_getLP1(&sensor1);
		
		sprintf(buffer,"%d %d %d %d %d %d %d %d\n",object0,motion0,presence0,LP1_0,LP2_0,LP2FR_0,presence,motion);
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


