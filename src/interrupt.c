#include "interrupt.h"
#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"

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
        SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk); // Enable Systick timer
        toggle_Led(LED2); // Toggle led to test interrupt
        vl53_enable++;
        vl53_enable = vl53_enable % 2;
        LoRa_Send_String((uint8_t *)"button press!\r\n"); // Send via LoRa
    }
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN); // Let HAL clear the pending interrupt
}

void I2C_IT_HANDLER(void)
{
    if(__HAL_GPIO_EXTI_GET_FLAG(I2C_INT_PIN)) 
    {
        toggle_Led(LED2); // Toggle led to test interrupt
        UART_PutStr("KAKAKFASDKFASFAR");
    }
    HAL_GPIO_EXTI_IRQHandler(I2C_INT_PIN);  // Let HAL clear the pending interrupt
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
    UART_PutStr("frikandelletjes\r\n");
    I2C_IT_HANDLER();
#else
    // Handlers which use EXTI4_15 on lora board
#endif
}
