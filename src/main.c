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

// Global variables
uint8_t vl53_enable = 0;  // Boolean, set to start counting with VL53

// Main function
int main(void)
{
    // Init functions
    HAL_Init();
    init_Leds();
    UART_Init();
    init_Button();
    HW_SPI_Init();
    LoRa_Tx_Init();
    I2C_Init();
    calipile_init();
    VL53_setup();

    #ifdef DEBUG
    UART_PutStr("Init complete\r\n");
    #endif
    
    // Uncomment function below if CaliPile is not used
    //vl53_enable = 1;  // Enable VL53

    change_network_timing_protocol(SEND_CONSTANT_FREQUENCY);

    while (1)
    {   
        // Check if calipile senses presence
        if(calipile_get_presence() > 100)
        {
            vl53_enable = 1;        // Set vl53_enable variable
            #ifdef DEBUG
            UART_PutStr("Wake up\r\n");
            #endif

        }
        if(vl53_enable) 
        {
            VL53_start_measuring(); // Start measuring
        }
    }
    return 0;
}
