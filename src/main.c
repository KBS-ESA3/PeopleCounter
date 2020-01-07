#include <stdlib.h>
#include "SPI.h"
#include "LoRa.h"
#include "interrupt.h"
#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "calipile_discovery.h"
#include "VL53L1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

#define CALIPILE_0_ADDR (0x0C << 1)
#define CALIPILE_1_ADDR (0x0D << 1)
#define CALIPILE_LOWPASS1 11
#define CALIPILE_LOWPASS2 8
#define CALIPILE_LOWPASS3 10
#define CALIPILE_PRTRSHLD 50
#define CALIPILE_MTNTRSHLD 10
#define CALIPILE_AMBTRSHLD 10
#define CALIPILE_INTERRUPT 0X00
#define CALIPILE_CYCLETIME 0X08
#define CALIPILE_SOURCE 0X01
#define CALIPILE_INTDIR 0X00
#define CALIPILE_INTTIM 0
#define CALIPILE_OBJTRSHLD 0

calipile_t sensor0 = {
    CALIPILE_0_ADDR,
    CALIPILE_LOWPASS1,
    CALIPILE_LOWPASS2,
    CALIPILE_LOWPASS3,
    CALIPILE_PRTRSHLD,
    CALIPILE_MTNTRSHLD,
    CALIPILE_AMBTRSHLD,
    CALIPILE_INTERRUPT,
    CALIPILE_CYCLETIME,
    CALIPILE_SOURCE,
    CALIPILE_INTDIR,
    CALIPILE_INTTIM,
    CALIPILE_OBJTRSHLD};
calipile_t sensor1 = {
    CALIPILE_1_ADDR,
    CALIPILE_LOWPASS1,
    CALIPILE_LOWPASS2,
    CALIPILE_LOWPASS3,
    CALIPILE_PRTRSHLD,
    CALIPILE_MTNTRSHLD,
    CALIPILE_AMBTRSHLD,
    CALIPILE_INTERRUPT,
    CALIPILE_CYCLETIME,
    CALIPILE_SOURCE,
    CALIPILE_INTDIR,
    CALIPILE_INTTIM,
    CALIPILE_OBJTRSHLD};

int main(void)
{
    HAL_Init();
    initLeds();
    UART_Init();
    //I2C_Init();
    HW_SPI_Init();
    LoRa_Tx_Init();
    calipile_init(&sensor0);
    UART_clearScreen();
    UART_PutStr("motion\n");

    while (1)
    {
        int16_t motion = 0;
        motion = calipile_getTPMotion(&sensor0);
        UART_PutInt(motion);
        UART_PutStr("\n");
        HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
        HAL_Delay(1000);

        LoRa_Send_String((uint8_t *)"Test via LoRa"); // send via LoRa
    }
}
