// Includes
#include "LoRa.h"
#include "SPI.h"
#include "board_definitions.h"
#include "calipile_discovery.h"
#include "error_handling.h"
#include "hardware_functions.h"
#include "interrupt.h"
#include "network.h"
#include "vl53_main.h"

#define CALIPILE_0_ADDR (0x0C << 1)
#define CALIPILE_1_ADDR (0x0D << 1)
#define CALIPILE_LOWPASS1 11
#define CALIPILE_LOWPASS2 8
#define CALIPILE_LOWPASS3 10
#define CALIPILE_PRTRSHLD 100
#define CALIPILE_MTNTRSHLD 10
#define CALIPILE_AMBTRSHLD 10
#define CALIPILE_INTERRUPT 0x08
#define CALIPILE_CYCLETIME 0X08
#define CALIPILE_SOURCE 0X01
#define CALIPILE_INTDIR 0X00
#define CALIPILE_INTTIM 0
#define CALIPILE_OBJTRSHLD 0

// Global variables
uint8_t vl53_enable = 0;  // Boolean, set to start counting with VL53

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

    uint16_t temp = 0;
    // Init functions
    HAL_Init();
    init_Leds();
    UART_Init();
    init_Button();
    HW_SPI_Init();
    LoRa_Tx_Init();
    I2C_Init();
    calipile_init(&sensor1);
    VL53_setup();
    init_sensor_interrupt();
    // Should be triggered by interrupt, uncomment line if not imlemented
    //vl53_enable = 1;  // Enable VL53
    change_network_timing_protocol(SEND_CONSTANT_FREQUENCY);
    while (1)
    {
        UART_PutInt(calipile_getTPPresence(&sensor1));
        UART_PutStr("       ");
        temp = calipile_ReadData(&sensor1, 18)&8;
        UART_PutInt(temp);
        UART_PutStr("\r\n");
        HAL_Delay(1000);
        
        if(0) 
        {
            #ifdef DEBUG
            UART_PutStr("Sensor wake up\r\n");
            #endif
            VL53_start_measuring();
        }
        #ifdef DEBUG 
        //UART_PutStr("Sensor put to sleep\r\n");
        #endif
        // Turn of Systick to prevent it Systck Interrupt to wake up the MCU again
        //SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
        //power_Sleep();
    }
    return 0;
}
