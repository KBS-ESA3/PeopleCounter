#ifndef LORA_H
#define LORA_H

#include "board_definitions.h"
#include "hardware_functions.h"
#include "SPI.h"

void LoRa_Tx_Init(void);
void Set_Frequency_LoRa(void);

void SX1276_Write(uint16_t addr, uint8_t data); // Write a single byte to sx1276 register adress over SPI.
uint8_t SX1276_Read(uint16_t addr);             // Read a single byte from sx1276 register over SPI.
void SX1276_Set_OpMode(uint8_t opMode);
/*
    OpModes SX1276:
    RFLR_OPMODE_SLEEP       - Set SX1276 to Sleep, enables to make changes to the SX1276 register
    RFLR_OPMODE_STANDBY     - Default mode
    RFLR_OPMODE_TRANSMITTER - Mode for transmitting data
    RFLR_OPMODE_RECEIVER    - Mode for receiving data
*/

void LoRa_Send_String(uint8_t *text_to_send); // Send string via LoRa.
void LoRa_Send_Word(uint16_t word);         // Send a word(16b) via LoRa.

#endif /* LORA_H */
