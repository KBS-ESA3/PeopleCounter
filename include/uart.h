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

#define BUFFER_SIZE 100     // For printing integers

UART_HandleTypeDef initStructure;

// Function declaration
void UART_Init();
void UART_GPIO_Init(void);
void UART_PrintStr(char *text);
void UART_PrintInt(uint32_t value);
void Error_Blink(void);

#endif