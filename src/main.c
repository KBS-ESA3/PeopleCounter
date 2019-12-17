/************************************
 *         Main source file         *
 ************************************/
// Includes
#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "calipile_discovery.h"
#include "vl53_main.h"

// Defines
#define CALIPILE_0_ADDR (0x0C << 1)
#define CALIPILE_1_ADDR (0x0D << 1)

// Uncomment the line below to enter debug mode
#define debug

// Declare Calipile objects for each sensor
Calipile sensor0 = {CALIPILE_0_ADDR, 11, 8, 10, 50, 10, 10, 0x00, 0x08, 0x01, 0x00, 0, 0};
Calipile sensor1 = {CALIPILE_1_ADDR, 11, 8, 10, 50, 10, 10, 0x00, 0x08, 0x01, 0x00, 0, 0};


int main(void)
{
  // Init functions
  HAL_Init();
  initLeds();
  UART_Init();
  I2C_Init();
  calipile_init(&sensor0);
  UART_clearScreen();
  VL53_setup();

  while (1)
  {
    VL53_get_distance();
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
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
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
