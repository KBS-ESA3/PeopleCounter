#ifndef SPI_H
#define SPI_H

#include "board_definitions.h"
#include "hardware_functions.h"

void HW_SPI_IoInit(void);
void HW_SPI_Init(void);
uint16_t HW_SPI_InOut(uint16_t txData);
static uint32_t SpiFrequency(uint32_t hz);
void HW_GPIO_Init(GPIO_TypeDef *port, uint16_t GPIO_Pin, GPIO_InitTypeDef *initStruct);
void Error_Handler(void);

#endif /* SPI_H */
