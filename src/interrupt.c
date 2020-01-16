#include "interrupt.h"
#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "network.h"
#include "packet.h"

/******************************************************************************/
/*                       User interrupt handlers                              */
/******************************************************************************/

/**
  * @brief handler for pressing the user button.
  */
void USER_BUTTON_IT_HANDLER(void)
{
    if (__HAL_GPIO_EXTI_GET_FLAG(USER_BUTTON_PIN))
    {
        toggle_Led(LED2);   // Toggle led to test interrupt
    }
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);  // Let HAL clear the pending interrupt
}

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

/**
  * @brief This function handles external interrupt channel 0 and 1.
  */
void EXTI0_1_IRQHandler()
{
#ifdef DISCO_BOARD
    // Handlers which use EXTI0_1 on discovery board
    USER_BUTTON_IT_HANDLER();
#else
    // Handlers which use EXTI0_1 on lora board
#endif
}

// This interrupt is from the network protocol SEND_AFTER_INACTIVE_PERIOD
// and is customizable in network.h
void TIM7_IRQHandler(void)
{
    #ifdef LORA_BOARD
    TIM_HandleTypeDef timer;
    
    timer.Instance = TIM7;
    if(__HAL_TIM_GET_FLAG(&timer, TIM_FLAG_UPDATE) != RESET)
    {
        network_send_people_count();

        disable_inactive_timer();
    }
    __HAL_TIM_CLEAR_FLAG(&timer, TIM_IT_UPDATE);
    #endif /* LORA_BOARD */
}

// This interrupt is from the network protocol SEND_CONSTANT_FREQUENCY
// and is customizable in network.h
void TIM6_IRQHandler(void)
{
    #ifdef LORA_BOARD
    TIM_HandleTypeDef timer;
    timer.Instance = TIM6;

    if(__HAL_TIM_GET_FLAG(&timer, TIM_FLAG_UPDATE) != RESET)
    {
        network_send_people_count();
    }
    __HAL_TIM_CLEAR_FLAG(&timer, TIM_IT_UPDATE);
    #else
    // This board does not support LoRa
    #endif /* LORA_BOARD */

}

void TIM2_IRQHandler(void)
{
    #ifdef LORA_BOARD
    TIM_HandleTypeDef timer;
    timer.Instance = TIM2;

    if(__HAL_TIM_GET_FLAG(&timer, TIM_FLAG_UPDATE) != RESET)
    {
        // ENABLE DOWNLINK MESSAGE!!!!!
        #ifdef DEBUG
        UART_PutStr("receiving lora");
        #endif /* DEBUG */
    }
    __HAL_TIM_CLEAR_FLAG(&timer, TIM_IT_UPDATE);
    #else
    // This board does not support LoRa
    #endif /* LORA_BOARD */

}

/**
  * @brief This function handles external interrupt channel 2 and 3.
  */
void EXTI2_3_IRQHandler()
{
#ifdef DISCO_BOARD
    // Handlers which use EXTI2_3 on discovery board
#else
    // Handlers which use EXTI2_3 on lora board
    USER_BUTTON_IT_HANDLER();
#endif
}

/**
  * @brief This function handles external interrupt channel 4 to 15.
  */
void EXTI4_15_IRQHandler()
{
#ifdef DISCO_BOARD
    // Handlers which use EXTI4_15 on discovery board
#else
    // Handlers which use EXTI4_15 on lora board
#endif
}
