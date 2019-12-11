#ifndef __STM32L0xx_IT_H
#define __STM32L0xx_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI0_1_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/