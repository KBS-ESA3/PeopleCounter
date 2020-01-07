/************************************
 *         Main source file         *
 ************************************/

// Includes
#include <stdlib.h>
#include "SPI.h"
#include "LoRa.h"
#include "interrupt.h"
#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "calipile_discovery.h"
#include "vl53_main.h"

#define CALIPILE_0_ADDR (0x0C<<1)
#define CALIPILE_1_ADDR (0x0D<<1)
#define CALIPILE_LOWPASS1 11
#define CALIPILE_LOWPASS2 8
#define CALIPILE_LOWPASS3 10
#define CALIPILE_PRTRSHLD 50
#define CALIPILE_MTNTRSHLD 10
#define CALIPILE_AMBTRSHLD 10
#define CALIPILE_INTERRUPT 0X00
#define CALIPILE_CYCLETIME 0X08
#define CALIPILE_SOURCE 0X01
#define CALIPILE_INTDIR 0X00
#define CALIPILE_INTTIM 0
#define CALIPILE_OBJTRSHLD 0

// Global variables
int16_t PeopleCount = 0;  // Should never be less than 0

calipile_t sensor0 = {
  CALIPILE_0_ADDR,
  CALIPILE_LOWPASS1,
  CALIPILE_LOWPASS2,
  CALIPILE_LOWPASS3,
  CALIPILE_PRTRSHLD,
  CALIPILE_MTNTRSHLD,
  CALIPILE_AMBTRSHLD,
  CALIPILE_INTERRUPT,
  CALIPILE_CYCLETIME,
  CALIPILE_SOURCE,
  CALIPILE_INTDIR,
  CALIPILE_INTTIM,
  CALIPILE_OBJTRSHLD
};
calipile_t sensor1 = {
  CALIPILE_1_ADDR,
  CALIPILE_LOWPASS1,
  CALIPILE_LOWPASS2,
  CALIPILE_LOWPASS3,
  CALIPILE_PRTRSHLD,
  CALIPILE_MTNTRSHLD,
  CALIPILE_AMBTRSHLD,
  CALIPILE_INTERRUPT,
  CALIPILE_CYCLETIME,
  CALIPILE_SOURCE,
  CALIPILE_INTDIR,
  CALIPILE_INTTIM,
  CALIPILE_OBJTRSHLD
};

int main(void)
{
  // Init functions
  HAL_Init();
  initLeds();
  UART_Init();
  //I2C_Init();
  HW_SPI_Init();
  LoRa_Tx_Init();
  calipile_init(&sensor0);
  //UART_clearScreen(); // Not needed with bluetooth debugging
  VL53_Setup();

  while (1)
  {
    start_measuring();
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
