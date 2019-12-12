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

#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE 16     // For printing integers
#define UART_MAX_TIMEOUT 0xFFFF
#define DECIMAL 10              // Parameter for the itoa() function to use the decimal system

UART_HandleTypeDef initStructure;

// Function declarations
void UART_Init();
void UART_PrintStr(char *string);
void UART_PrintInt(uint32_t value);
uint8_t Get_Strlen(char *string);

#endif  /* UART_H */
