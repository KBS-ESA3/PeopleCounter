#ifndef LORA_H
#define LORA_H

#include <stdint.h>

void LoRa_Tx_Init(void);
void Set_Frequency_LoRa(void);

void SX1276Write(uint16_t addr, uint8_t data); // Write to SX1276 register
uint8_t SX1276Read(uint16_t addr); // Read from SX1276 register
void SX1276SetOpMode(uint8_t opMode); // set Op mode SX1276

void LoRa_Send_String(uint8_t *textToSend); // Send string via LoRa

#endif /* LORA_H */
