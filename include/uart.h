/********************************************************************************
*   How to use this lib:                                                        *
*                                                                               *
*   - First, initialize UART by calling the UART_Init() function                *
*   - For printing an integer, call the UART_PrintInt(*your integer*) function  *
*   - For printing a string, call the UART_PrintStr(*your string*) function     *
********************************************************************************/
#ifndef UART_H
#define UART_H

#include "stm32l0xx_hal.h"
#include "conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UART_BUFFER_SIZE 16     // For printing integers

UART_HandleTypeDef initStructure;

// Function declarations
void UART_Init();
void UART_GPIO_Init(void);
void UART_PrintStr(char *text);
void UART_PrintInt(uint32_t value);
uint8_t Get_Strlen(char *string);
void Error_Blink(void);

#endif