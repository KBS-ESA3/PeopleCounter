#include "hardware_functions.h"

UART_HandleTypeDef UART_Handler;
I2C_HandleTypeDef I2C_Handler;

void init_Leds(void)
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

void init_Button(void)
{
    USER_BUTTON_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Pin = USER_BUTTON_PIN;
    HAL_GPIO_Init(USER_BUTTON_GPIO_PORT, &GPIO_InitStruct);
    HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);
}

void toggle_Led(uint8_t led)
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
        ERROR_send_warning(message);
        break;
    }
}

void set_Led(uint8_t led, uint8_t state)
{
    char message[] = "ledx is not available on this board!\n\n";
    switch (led)
    {
    case 1:
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, state);
        break;
    case 2:
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, state);
        break;
#ifdef LORA_BOARD
    case 3:
        HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, state);
        break;
    case 4:
        HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, state);
        break;
#endif
    default:
        message[3] = led;
        ERROR_send_warning(message);
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
    UART_Handler.Instance = USART1;
    UART_Handler.Init.BaudRate = UART_BAUD_RATE;
    UART_Handler.Init.Mode = UART_MODE_TX_RX;
    UART_Handler.Init.Parity = UART_PARITY_NONE;
    UART_Handler.Init.StopBits = UART_STOPBITS_1;
    UART_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&UART_Handler);
}

void UART_PutStr(char *message)
{
    uint8_t size = 0;
    while (message[size] != '\0')
    {
        size++;
    }
    HAL_UART_Transmit(&UART_Handler, message, size, HAL_UART_TIMEOUT_VALUE);
}

void UART_Putc(char c)
{
    HAL_UART_Transmit(&UART_Handler, c, 1, HAL_UART_TIMEOUT_VALUE);
}

void UART_PutInt(uint32_t val)
{
    char message[UART_INT_BUFFER];
    uint8_t size = 0;
    itoa(val, message, ITOA_DECIMAL);
    while (message[size] != '\0')
    {
        size++;
    }
    HAL_UART_Transmit(&UART_Handler, message, size, HAL_UART_TIMEOUT_VALUE);
}

void UART_clearScreen(void)
{
    char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
    char cmd2[4] = {0x1B, '[', 'f', '\0'};      // Cursor home

    UART_PutStr(cmd1);
    UART_PutStr(cmd2);
}

void UART_PutByte(uint8_t byte)
{
    int i;
    char result[11] = { '0','0','0','0',
                        '0','0','0','0',
                        '\n','\r','\0',};
    
    for(i=0; i < 8; i++)
    {
        result[i] = (byte & 1 << (7-i)) ? '1' : '0'; 
    }
    UART_PutStr(result);
}

void UART_PutWord(uint16_t word)
{
    int i;
    char result[19] = { '0','0','0','0',
                        '0','0','0','0',
                        '0','0','0','0',
                        '0','0','0','0',
                        '\n','\r','\0',};
    
    for(i=0; i < 16; i++)
    {
        result[i] = (word & 1 << (15-i)) ? '1' : '0'; 
    }
    UART_PutStr(result);
}

void UART_put_LoRaPacket(LoRa_packet_t packet)
{
    // Print device ID.
    UART_PutStr("Device id: ");
    UART_PutInt(packet.device_id);

    if(packet.type == TYPE_PEOPLE_COUNT)
    {
        // Output the number of people.
        UART_PutStr(" People count: ");
        UART_PutInt(packet.number_of_people);
    } 
    else if (packet.type = TYPE_BATTERY_STATUS) {
        // This must be the battery status.
        UART_PutStr(" Battery status: ");
        switch (packet.battery_status)
        {
            case BATTERY_GOOD:
                UART_PutStr("Good!");
                break;
            case BATTERY_LOW:
                UART_PutStr("low");
                break;
            case BATTERY_CRITICAL:
                UART_PutStr("critical!");
                break;
            default:
            // Invalid data.
                UART_PutStr("invalid packet!!");
                break;
        }
    }
    else if(packet.type == TYPE_SWITCH_ALGORITHM)
    {
        UART_PutStr(" Switch method packet: ");
        switch (packet.network_protocol)
        {
            case SEND_CONSTANT_FREQUENCY:
                UART_PutStr("send constant frequency");
                break;
            case SEND_EVERY_PASSAGE:
                UART_PutStr("send every passage");
                break;
            case SEND_PEAK_AT_ZERO:
                UART_PutStr("send peak at zero");
                break;
            case SEND_AFTER_INACTIVE_PERIOD:
                UART_PutStr("send inactive period");
                break;
            default:
            // invalid data.
                break;
        }
    }
    UART_PutStr("\n\r");
}

uint8_t Get_Strlen(char *string)
{
    uint8_t size = 0;
    for (uint8_t i = 0; string[i] != '\0'; i++) // Get the size of string
    {
        size++;
    }
    return size;
}

void I2C_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Alternate = I2C_GPIO_AF;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pin = I2C_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = I2C_CLK_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
    I2C_Handler.Instance = I2C1;
    I2C_Handler.Init.Timing = I2C_TIMING;
    I2C_Handler.Init.OwnAddress1 = 0;
    I2C_Handler.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2C_Handler.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2C_Handler.Init.OwnAddress2 = 0;
    I2C_Handler.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2C_Handler.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&I2C_Handler);
}

void I2C_Write(uint8_t adress, uint8_t *data, uint8_t size)
{
    HAL_I2C_Master_Transmit(&I2C_Handler, adress, data, size, HAL_MAX_DELAY);
}

void I2C_Read(uint8_t adress, uint8_t *destination, uint8_t size)
{
    HAL_I2C_Master_Receive(&I2C_Handler, adress, destination, size, HAL_MAX_DELAY);
}

void I2C_WriteRegister8(uint8_t adress, uint8_t regg, uint8_t *data, uint8_t size)
{
    HAL_I2C_Mem_Write(&I2C_Handler, adress, regg, 1, data, size, HAL_MAX_DELAY);
}

void I2C_WriteRegister16(uint8_t adress, uint16_t regg, uint8_t *data, uint8_t size)
{
    HAL_I2C_Mem_Write(&I2C_Handler, adress, regg, 2, data, size, HAL_MAX_DELAY);
}

void I2C_ReadRegister8(uint8_t adress, uint8_t regg, uint8_t *destination, uint8_t size)
{
    HAL_I2C_Mem_Read(&I2C_Handler, adress, regg, 1, destination, size, HAL_MAX_DELAY);
}

void I2C_ReadRegister16(uint8_t adress, uint16_t regg, uint8_t *destination, uint8_t size)
{
    HAL_I2C_Mem_Read(&I2C_Handler, adress, regg, 2, destination, size, HAL_MAX_DELAY);
}

/**
  * @brief Puts the microcontroller in sleep mode.
  * Only cpu clock is stopped.
  */
void power_Sleep()
{
    set_Led(LED1, LED_OFF);
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

/**
  * @brief Puts the microcontroller in stop mode.
  * The cpu and peripheral clocks are stopped only wakes up on external interrupts or communication interrupts.
  */
void power_Deepsleep()
{
    set_Led(LED1, LED_OFF);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/**
  * @brief Puts the microcontroller in standby mode.
  * Stops the microcontroller and shuts down all peripherals.
  * Only wakes up on wakeup pins or reset.
  * Processor starts at beginning of main on wakeup.
  */
void power_Off()
{
    set_Led(LED1, LED_OFF);
    HAL_PWR_EnterSTANDBYMode();
}
