#ifndef LORA_H
#define LORA_H

#include <stdint.h>

// LoRa
void LoRa_Tx_Init(void);
void Set_Frequency_LoRa(void);

void SX1276Write(uint16_t addr, uint8_t data);
uint8_t SX1276Read(uint16_t addr);
void SX1276WriteBuffer(uint16_t addr, uint8_t *buffer, uint8_t size);
void SX1276ReadBuffer(uint16_t addr, uint8_t *buffer, uint8_t size);
void SX1276WriteFifo(uint8_t *buffer, uint8_t size); 
void SX1276ReadFifo(uint8_t *buffer, uint8_t size);
void SX1276SetOpMode(uint8_t opMode);

// Radio Send
void SX1276Send(uint8_t *buffer, uint8_t size);

#endif /* LORA_H */
