/******************************************************************************
 * File           : Main program - I2C 2x16 LCD BTHQ21605V
 *****************************************************************************/
#include "config.h"


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


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
	char buffer[100];
	uint32_t object0 = 0;
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
		motion0 = calipile_getTPMotion(&sensor1);
		presence0 = calipile_getTPPresence(&sensor1);
		LP1_0 = calipile_getLP1(&sensor1);
		
		sprintf(buffer,"%d %d %d %d %d %d %d %d\n",object0,motion0,presence0,LP1_0,LP2_0,LP2FR_0,presence,motion);
		USART_putstr(buffer);
		Delay(DELAY_MS*10);
  }
}





