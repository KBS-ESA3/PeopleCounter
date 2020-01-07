#ifndef LORA_H
#define LORA_H

#include "board_definitions.h"
#include "hardware_functions.h"
#include "SPI.h"

void LoRa_Tx_Init(void);
void Set_Frequency_LoRa(void);

void SX1276_Write(uint16_t addr, uint8_t data); // write a single byte to sx1276 register adress over spi
uint8_t SX1276_Read(uint16_t addr);             // read a single byte from sx1276 register over spi
void SX1276_Set_OpMode(uint8_t opMode);
/*
    OpModes SX1276:
    RFLR_OPMODE_SLEEP       - Set SX1276 to Sleep, enables to make changes to the SX1276 register
    RFLR_OPMODE_STANDBY     - Default mode
    RFLR_OPMODE_TRANSMITTER - Mode for transmitting data
    RFLR_OPMODE_RECEIVER    - Mode for receiving data
*/

void LoRa_Send_String(uint8_t *textToSend); // Send string via LoRa

#endif /* LORA_H */
