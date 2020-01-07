#include "interrupt.h"
#include "hardware_functions.h"
#include "network.h"
/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 and line 1 interrupts.
  */
void EXTI0_1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
void TIM7_IRQHandler(void)
{

    TIM_HandleTypeDef timer;
    
    timer.Instance = TIM7;
    if(__HAL_TIM_GET_FLAG(&timer, TIM_FLAG_UPDATE) != RESET){
        network_send_people_count();

        disable_inactive_timer();
    }
    __HAL_TIM_CLEAR_FLAG(&timer, TIM_IT_UPDATE);
}

void TIM6_IRQHandler(void)
{
    TIM_HandleTypeDef timer;
    timer.Instance = TIM6;

    if(__HAL_TIM_GET_FLAG(&timer, TIM_FLAG_UPDATE) != RESET){
        network_send_people_count();

    }
    __HAL_TIM_CLEAR_FLAG(&timer, TIM_IT_UPDATE);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
