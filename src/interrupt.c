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
        toggle_Led(LED2);
        UART_PutStr("Can't touch this!\r\n");
    }
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN); //let HAL clear the pending interrupt
}

/**
  * @brief handler for calipile interrupt pin.
  */
void CALIPILE_INTERRUPT_HANDLER(void)
{
    //TODO nog implementeren
}

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    set_Led(LED1, LED_ON);
    HAL_IncTick();
}

/**
  * @brief This function handles external interrupt channel 0 and 1.
  */
void EXTI0_1_IRQHandler()
{
#ifdef DISCO_BOARD
    //handlers wich use EXTI0_1 on discovery board
    USER_BUTTON_IT_HANDLER();
#else
    //handlers wich use EXTI0_1 on lora board
#endif
}

/**
  * @brief This function handles external interrupt channel 2 and 3.
  */
void EXTI2_3_IRQHandler()
{
#ifdef DISCO_BOARD
    //handlers wich use EXTI2_3 on discovery board
#else
    //handlers wich use EXTI2_3 on lora board
    USER_BUTTON_IT_HANDLER();
#endif
}

/**
  * @brief This function handles external interrupt channel 4 to 15.
  */
void EXTI4_15_IRQHandler()
{
#ifdef DISCO_BOARD
    //handlers wich use EXTI4_15 on discovery board
#else
    //handlers wich use EXTI4_15 on lora board
#endif
}
