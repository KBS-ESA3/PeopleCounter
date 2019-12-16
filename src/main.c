#include "board_definitions.h"
#include "hardware_functions.h"



int main(void)
{
  HAL_Init();
  initLeds();
  UART_Init();
  UART_clearScreen();
  UART_Hello();
  UART_PutStr("world \n\r");
  UART_PutInt(69);

  while (1)
  {
    HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    HAL_Delay(200);
    
    
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


