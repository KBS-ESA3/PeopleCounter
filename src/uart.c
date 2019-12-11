#include "uart.h"

void UART_GPIO_Init(void);
void Error_Blink(void);

void UART_Init(void)
{
    USARTx_CLK_ENABLE();

    UART_GPIO_Init();

    initStructure.Instance = USART2;
    initStructure.Init.BaudRate = 115200;
    initStructure.Init.Mode = UART_MODE_TX_RX;
    initStructure.Init.Parity = UART_PARITY_NONE;
    initStructure.Init.StopBits = UART_STOPBITS_1;
    initStructure.Init.WordLength = UART_WORDLENGTH_8B;
    initStructure.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    if (HAL_UART_Init(&initStructure) != HAL_OK)    // If error..
    {
        Error_Blink();  // Blink a LED
    }
}

void UART_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // Enable GPIO TX/RX clock
    USARTx_RTX_GPIO_CLK_ENABLE();
    // UART TX GPIO pin configuration
    GPIO_InitStruct.Pin = USARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    // UART RX GPIO pin configuration
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
}

void UART_PrintStr(char *string)
{
    HAL_UART_Transmit(&initStructure, (uint8_t*)string, Get_Strlen(string), 0xFFFF);
}

void UART_PrintInt(uint32_t value)
{
    char bufferArray[UART_BUFFER_SIZE] = "";

    itoa(value, bufferArray, 10); // Transfer int to char and put it in bufferArray
    HAL_UART_Transmit(&initStructure, (uint8_t*)bufferArray, Get_Strlen(bufferArray), 0xFFFF);
}

uint8_t Get_Strlen(char *string)
{
    uint8_t size = 0;
    for(uint8_t i = 0; string[i] != '\0'; i++)  // Get the size of bufferArray
    {
        size++;
    }
    
    return size;
}

void Error_Blink(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    // Initialize pin b6 (blue LED)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    while (1)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
        HAL_Delay(100);
    }
}
