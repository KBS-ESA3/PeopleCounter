#ifndef __HARDWARE_FUNCTIONS_H
#define __HARDWARE_FUNCTIONS_H

#include "stdint.h"
#include "error_handling.h"

#define UART_INT_BUFFER 10
#define print UART_PutStr

void initLeds(void);
void toggleLed(uint8_t led);

void UART_Init();
void UART_clearScreen(void);
void UART_Hello(void);
void UART_Putc(char c);
void UART_PutStr(char* message);
void UART_PutInt(uint32_t val);

void I2C_Init();
void I2C_Write(uint8_t adress, uint8_t* data,uint8_t size);
void I2C_Read(uint8_t adress, uint8_t* destination,uint8_t size);
void I2C_WriteRegister8(uint8_t adress, uint8_t regg, uint8_t* data, uint8_t size);
void I2C_WriteRegister16(uint8_t adress, uint16_t regg, uint8_t* data, uint8_t size);
void I2C_ReadRegister8(uint8_t adress, uint8_t regg, uint8_t* destination, uint8_t size);
void I2C_ReadRegister16(uint8_t adress, uint16_t regg, uint8_t* destination, uint8_t size);

#endif /* __HARDWARE_FUNCTIONS_H */

