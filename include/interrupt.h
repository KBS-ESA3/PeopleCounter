#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "board_definitions.h"

void SysTick_Handler(void);
void EXTI0_1_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
