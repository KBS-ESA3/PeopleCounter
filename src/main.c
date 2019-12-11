#include "uart.h"

int main(void)
{
    HAL_Init();
    UART_Init();

    while (1)
    {
        UART_PrintInt(1234);
        UART_PrintStr("Test");
        HAL_Delay(1000);
    }
}