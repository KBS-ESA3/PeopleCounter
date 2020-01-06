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
    I2C_Init();
    calipile_init(&sensor0);
    calipile_init(&sensor1);
    UART_clearScreen();
    UART_PutStr("motion\n");
    // int16_t motion = 0;
    int16_t object = 0;
    int16_t objectOld = 0;
    int16_t objectDif = 0;
    int16_t objectDifOld = 0;
    int16_t objectDifDif = 0;
    int16_t topPeak = 0;
    int16_t botPeak = 0;

    int16_t object1 = 0;
    int16_t objectOld1 = 0;
    int16_t objectDif1 = 0;
    int16_t objectDifOld1 = 0;
    int16_t objectDifDif1 = 0;
    int16_t topPeak1 = 0;
    int16_t botPeak1 = 0;

    int8_t step = 0;
    int16_t people = 0;

    while (1)
    {
        // int16_t motionOld = motion;
        objectOld = object;
        object = calipile_getTPObject(&sensor0);
        objectDifOld = objectDif;
        objectDif = object - objectOld;
        objectDifDif = objectDif - objectDifOld;

        objectOld1 = object1;
        object1 = calipile_getTPObject(&sensor1);
        objectDifOld1 = objectDif1;
        objectDif1 = object1 - objectOld1;
        objectDifDif1 = objectDif1 - objectDifOld1;
        // motion = calipile_getTPMotion(&sensor0);
        // UART_PutInt(motion - motionOld);
        // UART_PutStr(" ");
        // UART_PutInt(motion);
        // UART_PutStr(" ");
        // UART_PutInt(objectDifDif);
        // UART_PutStr(" ");
        // UART_PutInt(objectDif1);
        // UART_PutStr(" ");
        // UART_PutInt(object);
        // UART_PutStr(" ");
        // UART_PutInt(objectDif1 + 200);
        // UART_PutStr(" ");
        // UART_PutInt(object1);
        // UART_PutStr(" ");
        //UART_PutInt(object);
        UART_PutStr("\n");

        if((step == 0 || step == 1) && objectDif > 20){
            if(objectDif > topPeak){
                topPeak = objectDif;
            }
            step = 1; 
        }
        if((step == 1 || step == 2) && objectDif < -20){
            if(objectDif < botPeak){
                botPeak = objectDif;
            }
            step = 2;
        }
        if(step == 2 && objectDif > -20){
            if(topPeak > (botPeak * -1)){
                people++;
            }else{
                people--;
            }
            step = 0;
            topPeak = 0;
            botPeak = 0;
        }
        
        //UART_PutInt(people);
        UART_PutInt(objectDif);

        HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
        HAL_Delay(25);
    }
}
