
#include "hardware_functions.h"

void initLeds(void)
{

    LED1_GPIO_CLK_ENABLE();
    LED2_GPIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LED1_PIN;
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);
#ifdef LORA_BOARD
    GPIO_InitStruct.Pin = LED3_PIN;
    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED4_PIN;
    HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);
#endif
}

void toggleLed(uint8_t led)
{
    char message[] = "ledx is not available on this board!\n\n";
    switch (led)
    {

    case 1:
        HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
        break;
    case 2:
        HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
        break;
#ifdef LORA_BOARD
    case 3:
        HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
        break;
    case 4:
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        break;
#endif
    default:
        message[3] = led;
        sendWarning(message);
        break;
    }
}

void UART_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Alternate = UART_GPIO_AF;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStructure.Pin = UART_TX_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    HAL_GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = UART_RX_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(UART_GPIO_PORT, &GPIO_InitStructure);

    UART_Initstructure.Instance = USART1;
    UART_Initstructure.Init.BaudRate = 115200;
    UART_Initstructure.Init.Mode = UART_MODE_TX_RX;
    UART_Initstructure.Init.Parity = UART_PARITY_NONE;
    UART_Initstructure.Init.StopBits = UART_STOPBITS_1;
    UART_Initstructure.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Initstructure.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    HAL_UART_Init(&UART_Initstructure);
}

void UART_Hello(void)
{
    uint8_t message[] = "hello\n\r";
    HAL_UART_Transmit(&UART_Initstructure, message, sizeof(message), HAL_UART_TIMEOUT_VALUE);
}

void UART_PutStr(char *message)
{
    uint8_t size = 0;
    while (message[size] != '\0')
        size++;
    HAL_UART_Transmit(&UART_Initstructure, message, size, HAL_UART_TIMEOUT_VALUE);
}

void UART_Put(char c)
{
    HAL_UART_Transmit(&UART_Initstructure, c, 1, HAL_UART_TIMEOUT_VALUE);
}

void UART_PutInt(uint32_t val)
{
    char message[UART_INT_BUFFER];
    uint8_t size = 0;
    itoa(val,message,10);    
    while (message[size] != '\0')
        size++;
    HAL_UART_Transmit(&UART_Initstructure, message, size, HAL_UART_TIMEOUT_VALUE);
}

void UART_clearScreen(void)
{
    char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
    char cmd2[4] = {0x1B, '[', 'f', '\0'};      // Cursor home

    UART_PutStr(cmd1);
    UART_PutStr(cmd2);
}