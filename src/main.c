#include "uart.h"
#include "sx1276Regs-LoRa.h"
#include "SPI.h"
#include "LoRa.h"
#include <string.h>
#include <stdlib.h>

int main(void)
{
    uint8_t testLora[] = {'c'};

    HAL_Init();
    UART_Init();
    HW_SPI_Init();
    LoRa_Tx_Init();


    while (1)
    {
        UART_PrintInt(1234);
        UART_PrintStr("Test\n");
        HAL_Delay(1000);

        SX1276Send((uint8_t*)testLora, 1);
    }
}
