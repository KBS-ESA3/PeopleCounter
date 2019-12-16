#ifndef __HARDWARE_FUNCTIONS_H
#define __HARDWARE_FUNCTIONS_H

#include "stdint.h"
#include "error_handling.h"

#define UART_INT_BUFFER 10

UART_HandleTypeDef UART_Initstructure;

void initLeds(void);
void toggleLed(uint8_t led);
void UART_Init();
void UART_clearScreen(void);
void UART_Hello(void);
void UART_Putc(char c);
void UART_PutStr(char* message);
void UART_PutInt(uint32_t val);

#endif /* __HARDWARE_FUNCTIONS_H */

