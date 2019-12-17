/******************************************************************************
 * File           : global hardware communication functions
 *****************************************************************************/
#ifndef _HARDWARE_COMM_H_
#define _HARDWARE_COMM_H_

#include "config.h"

void I2C_Setup(void);
void I2C_sendData(uint8_t data);
uint8_t I2C_readData(void);
void I2C_startWrite(uint8_t adress, uint8_t bytes, uint8_t autoStop);
void I2C_startRead(uint8_t adress, uint8_t bytes, uint8_t autoStop);
void I2C_stop(void);
uint8_t I2C_checkTranferComplete(void);
void WaitForI2CFlag(uint32_t flag);

void USART_init(void);
void USART_putc(char c);
void USART_putstr(char *str);
void USART_putint(int16_t v);
void USART_clearscreen(void);



#endif /* _HARDWARE_COMM_H_ */
