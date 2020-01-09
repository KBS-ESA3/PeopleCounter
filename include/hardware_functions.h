#ifndef __HARDWARE_FUNCTIONS_H
#define __HARDWARE_FUNCTIONS_H

#include "stdint.h"
#include "error_handling.h"
#include "packet.h"

#define UART_INT_BUFFER 10
#define UART_BAUD_RATE 115200
#define ITOA_DECIMAL 10

UART_HandleTypeDef UART_Handler;
I2C_HandleTypeDef I2C_Handler;

void init_Leds(void);
void toggle_Led(uint8_t led);
void set_Led(uint8_t led, uint8_t state);
void init_Button(void);

void UART_Init();
void UART_clearScreen(void);
void UART_Putc(char c);
void UART_PutStr(char *message);
void UART_PutInt(uint32_t val);
void UART_PutWord(uint16_t Word);
void UART_PutByte(uint8_t Byte);
void UART_put_LoRaPacket(LoRa_packet_t packet);
uint8_t Get_Strlen(char *string);

void I2C_Init();
void I2C_Write(uint8_t adress, uint8_t *data, uint8_t size);
void I2C_Read(uint8_t adress, uint8_t *destination, uint8_t size);
void I2C_WriteRegister8(uint8_t adress, uint8_t regg, uint8_t *data, uint8_t size);
void I2C_WriteRegister16(uint8_t adress, uint16_t regg, uint8_t *data, uint8_t size);
void I2C_ReadRegister8(uint8_t adress, uint8_t regg, uint8_t *destination, uint8_t size);
void I2C_ReadRegister16(uint8_t adress, uint16_t regg, uint8_t *destination, uint8_t size);

void power_Sleep();
void power_Deepsleep();
void power_Off();

#endif /* __HARDWARE_FUNCTIONS_H */
