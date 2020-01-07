#ifndef __BOARD_DEFINITIONS_H
#define __BOARD_DEFINITIONS_H
#include "stdint.h"

#if DISCO_BOARD
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_gpio.h"
#include "stm32f0xx_hal_uart.h"
#include "stm32f0xx_hal_i2c.h"

#define LEDn 2

#define LED1_PIN GPIO_PIN_9
#define LED1_GPIO_PORT GPIOC
#define LED1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

#define LED2_PIN GPIO_PIN_8
#define LED2_GPIO_PORT GPIOC
#define LED2_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__, __INITSTRUCT__) \
  do                                                    \
  {                                                     \
    switch (__INDEX__)                                  \
    {                                                   \
    case LED1:                                          \
      LED1_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED1_GPIO_PORT, __INITSTRUCT__);    \
      break;                                            \
    case LED2:                                          \
      LED2_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED2_GPIO_PORT, __INITSTRUCT__);    \
      break;                                            \
    default:                                            \
      break;                                            \
    }                                                   \
  } while (0)

#define LEDx_GPIO_CLK_DISABLE(__INDEX__) \
  do                                     \
  {                                      \
    switch (__INDEX__)                   \
    {                                    \
    case LED1:                           \
      LED1_GPIO_CLK_DISABLE();           \
      break;                             \
    case LED2:                           \
      LED2_GPIO_CLK_DISABLE();           \
      break;                             \
    default:                             \
      break;                             \
    }                                    \
  } while (0)

#define I2C_CLK_PIN GPIO_PIN_6
#define I2C_SDA_PIN GPIO_PIN_7
#define I2C_GPIO_PORT GPIOB
#define I2C_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C_GPIO_AF GPIO_AF1_I2C1
#define I2C_TIMING 0x2033030A

#define UART_TX_PIN GPIO_PIN_9
#define UART_RX_PIN GPIO_PIN_10
#define UART_GPIO_PORT GPIOA
#define UART_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define UART_GPIO_AF GPIO_AF1_USART1

#define USER_BUTTON_PIN GPIO_PIN_0
#define USER_BUTTON_GPIO_PORT GPIOA
#define USER_BUTTON_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define USER_BUTTON_EXTI_LINE GPIO_PIN_0
#define USER_BUTTON_EXTI_IRQn EXTI0_1_IRQn

#elif LORA_BOARD
#include "stm32l0xx_hal.h"
#include "stm32l0xx_hal_gpio.h"
#include "stm32l0xx_hal_uart.h"
#include "stm32l0xx_hal_i2c.h"

#define LEDn 4

#define LED1_PIN GPIO_PIN_5
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED2_PIN GPIO_PIN_5
#define LED2_GPIO_PORT GPIOA
#define LED2_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()

#define LED3_PIN GPIO_PIN_6
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED4_PIN GPIO_PIN_7
#define LED4_GPIO_PORT GPIOB
#define LED4_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__, __INITSTRUCT__) \
  do                                                    \
  {                                                     \
    switch (__INDEX__)                                  \
    {                                                   \
    case LED1:                                          \
      LED1_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED1_GPIO_PORT, __INITSTRUCT__);    \
      break;                                            \
    case LED2:                                          \
      LED2_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED2_GPIO_PORT, __INITSTRUCT__);    \
      break;                                            \
    case LED3:                                          \
      LED3_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED3_GPIO_PORT, __INITSTRUCT__);    \
      break;                                            \
    case LED4:                                          \
      LED4_GPIO_CLK_ENABLE();                           \
      HAL_GPIO_Init(LED4GPIO_PORT, __INITSTRUCT__);     \
      break;                                            \
    default:                                            \
      break;                                            \
    }                                                   \
  } while (0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__) \
  do                                     \
  {                                      \
    switch (__INDEX__)                   \
    {                                    \
    case LED1:                           \
      LED1_GPIO_CLK_DISABLE();           \
      break;                             \
    case LED2:                           \
      LED2_GPIO_CLK_DISABLE();           \
      break;                             \
    case LED3:                           \
      LED3_GPIO_CLK_DISABLE();           \
      break;                             \
    case LED4:                           \
      LED4_GPIO_CLK_DISABLE();           \
      break;                             \
    default:                             \
      break;                             \
    }                                    \
  } while (0)

#define I2C_CLK_PIN GPIO_PIN_6
#define I2C_SDA_PIN GPIO_PIN_7
#define I2C_GPIO_PORT GPIOB
#define I2C_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()
#define I2C_GPIO_AF GPIO_AF1_I2C1
#define I2C_TIMING 0x2033030A

#define UART_TX_PIN GPIO_PIN_9
#define UART_RX_PIN GPIO_PIN_10
#define UART_GPIO_PORT GPIOA
#define UART_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define UART_GPIO_AF GPIO_AF4_USART1

#define USER_BUTTON_PIN GPIO_PIN_2
#define USER_BUTTON_GPIO_PORT GPIOB
#define USER_BUTTON_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE() __HAL_RCC_GPIOB_CLK_DISABLE()
#define USER_BUTTON_EXTI_LINE GPIO_PIN_2
#define USER_BUTTON_EXTI_IRQn EXTI2_3_IRQn


#else
#error "Unsupported STM32 Family"
#endif

#endif /* __BOARD_DEFINITIONS_H */
