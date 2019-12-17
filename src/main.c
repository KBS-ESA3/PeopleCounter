#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "calipile_discovery.h"
#include "VL53L1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"


#define CALIPILE_0_ADDR (0x0C<<1)
#define CALIPILE_1_ADDR (0x0D<<1)

Calipile sensor0 = {CALIPILE_0_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};
Calipile sensor1 = {CALIPILE_1_ADDR,11,8,10,50,10,10,0x00,0x08,0x01,0x00,0,0};

int main(void)
{
  HAL_Init();
  initLeds();
  UART_Init();
  I2C_Init();
  calipile_init(&sensor0);
  UART_clearScreen();
  UART_PutStr("motion\n");


  while (1)
  {
    int16_t motion = 0;
    motion = calipile_getTPMotion(&sensor0);
    UART_PutInt(motion);
    UART_PutStr("\n");
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    HAL_Delay(10);
    
    
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1) {}
}


void MemManage_Handler(void)
{
  while (1) {}
}

void BusFault_Handler(void)
{
  while (1) {}
}

void UsageFault_Handler(void)
{
  while (1) {}
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}


