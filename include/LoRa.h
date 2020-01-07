#ifndef LORA_H
#define LORA_H

#include <stdint.h>

void LoRa_Tx_Init(void);
void Set_Frequency_LoRa(void);

void SX1276_Write(uint16_t addr, uint8_t data); // write a single byte to sx1276 register adress over spi
uint8_t SX1276_Read(uint16_t addr);             // read a single byte from sx1276 register over spi
void SX1276_Set_OpMode(uint8_t opMode);
/*
    opModes:
    Sleep           - Set SX1276 to Sleep, enables to make changes to the SX1276 register
    Standy          - Default mode
    Transmitter     - Mode for transmitting data
    Receiver        - Mode for receiving data
*/

void LoRa_Send_String(uint8_t *textToSend); // Send string via LoRa

#endif /* LORA_H */
