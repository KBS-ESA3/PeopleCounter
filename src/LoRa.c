#include "LoRa.h"
#include "stm32l0xx_hal.h"
#include "sx1276Regs-LoRa.h"
#include "mlm32l0xx_hw_conf.h"

#define FREQ_STEP_8 15625
#define FREQUENCY_LORA 868000000

#define SX_FREQ_TO_CHANNEL( channel, freq )                                                                       \
    do                                                                                                            \
    {                                                                                                             \
        uint32_t initialFreqInt, initialFreqFrac;                                                                 \
        initialFreqInt = freq / FREQ_STEP_8;                                                                      \
        initialFreqFrac = freq - ( initialFreqInt * FREQ_STEP_8 );                                                \
        channel = ( initialFreqInt << 8 ) + ( ( ( initialFreqFrac << 8 ) + ( FREQ_STEP_8 / 2 ) ) / FREQ_STEP_8 ); \
    }while( 0 )

void LoRa_Tx_Init(void)
{
    // Sleep mode on to change settings
    SX1276Write( REG_LR_OPMODE, ( SX1276Read( REG_LR_OPMODE ) & RFLR_OPMODE_MASK ) | RFLR_OPMODE_SLEEP );

    /*
    Settings Init
    - LoRa Mode
    - Access share LoRa (default)
    - Low Frequency mode(default)
    - Sleep mode(for Setting of LoRa)
    - regfr Frequency LoRa 868000000 Hz
    - PaSelect - RFO (default)
    - MaxPower (default)
    - OutputPower (default)
    - PaRamp - 40 us(default)
    - OcpOn - disabled(default)
    - OcpTrim (default)
    - LnaGain - maximum gain(default)
    - LnaBoostLf (default)
    */

    // LoRa Mode on
    SX1276Write( REG_LR_OPMODE, ( SX1276Read( REG_LR_OPMODE ) & RFLR_OPMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON );

    // Set frequency LoRa
    Set_Frequency_LoRa();
}

void Set_Frequency_LoRa(void)
{
    uint32_t channel;

    // Set frequency LoRa                                                                         
    SX_FREQ_TO_CHANNEL( channel, FREQUENCY_LORA );
    SX1276Write( REG_LR_FRFMSB, ( uint8_t )( ( channel >> 16 ) & 0xFF ) );
    SX1276Write( REG_LR_FRFMID, ( uint8_t )( ( channel >> 8 ) & 0xFF ) );
    SX1276Write( REG_LR_FRFLSB, ( uint8_t )( channel & 0xFF ) );
}

void SX1276Write( uint16_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}


uint8_t SX1276Read( uint16_t addr )
{
    uint8_t data;
    SX1276ReadBuffer( addr, &data, 1 );
    return data;
}

void SX1276WriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, 0);

    HW_SPI_InOut( addr | 0x80 );
    for( i = 0; i < size; i++ )
    {
        HW_SPI_InOut( buffer[i] );
    }

    //NSS = 1;
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, 1);
}

void SX1276ReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, 0);

    HW_SPI_InOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = HW_SPI_InOut( 0 );
    }

    //NSS = 1;
    HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, 1);
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

void SX1276SetOpMode( uint8_t opMode )
{
    SX1276Write( REG_LR_OPMODE, ( SX1276Read( REG_LR_OPMODE ) & RFLR_OPMODE_MASK ) | opMode );
}

void SX1276Send(uint8_t *buffer, uint8_t size)
{
    // Set Standby Mode
    SX1276Write( REG_LR_OPMODE, ( SX1276Read( REG_LR_OPMODE ) & RFLR_OPMODE_MASK ) | RFLR_OPMODE_STANDBY );

    // Initializes the payload size
    SX1276Write( REG_LR_PAYLOADLENGTH, size );

    // Full buffer used for Tx
    SX1276Write(REG_LR_FIFOTXBASEADDR, 0);
    SX1276Write(REG_LR_FIFOADDRPTR, 0);

    // Write payload buffer
    SX1276WriteFifo(buffer, size);

    // Set TX Mode
    SX1276Write( REG_LR_OPMODE, ( SX1276Read( REG_LR_OPMODE ) & RFLR_OPMODE_MASK ) | RFLR_OPMODE_TRANSMITTER );
    // Polling if TX is done
    while((SX1276Read(REG_LR_IRQFLAGS) & RFLR_IRQFLAGS_TXDONE) == 0);

}

void LoRa_Send_String(uint8_t *textToSend)
{
    SX1276Send((uint8_t*)textToSend, Get_Strlen(textToSend));
}
