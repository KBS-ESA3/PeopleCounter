/************************************
 *       VL53 Main source file      *
 ************************************/

// Includes
#include "stm32f0xx_hal.h"
#include "vl53_main.h"
#include "hardware_functions.h"

// Programmabe I2C address of the VL53L1X sensor
uint16_t VL53_I2C_Address = (0x29 << 1);

char buffer[50];
uint16_t wordData, Distance, AmbientRate, SpadNum, SignalRate;
uint8_t status, byteData, sensorState = 0, dataReady, RangeStatus, ToFSensor = 1;

void draw_SPAD_grid()
{
}

void VL53_get_distance()
{
    status = VL53L1X_StartRanging(VL53_I2C_Address); /* This function has to be called to enable the ranging */
    while (1)
    { /* read and display data */
      while (dataReady == 0)
      {
        status = VL53L1X_CheckForDataReady(VL53_I2C_Address, &dataReady);
        HAL_Delay(2);
      }
      dataReady = 0;
      status = VL53L1X_GetRangeStatus(VL53_I2C_Address, &RangeStatus);
      status = VL53L1X_GetDistance(VL53_I2C_Address, &Distance);
      status = VL53L1X_GetSignalRate(VL53_I2C_Address, &SignalRate);
      status = VL53L1X_GetAmbientRate(VL53_I2C_Address, &AmbientRate);
      status = VL53L1X_GetSpadNb(VL53_I2C_Address, &SpadNum);
      status = VL53L1X_ClearInterrupt(VL53_I2C_Address); /* clear interrupt has to be called to enable next interrupt*/
        #ifdef debug
        sprintf(buffer, "%u, %u, %u, %u, %u\n\r", RangeStatus, Distance, SignalRate, AmbientRate, SpadNum);
        UART_PutStr(buffer);
        #endif
    }
}

void VL53_setup()
{
    // Wait for sensor to boot
    while (sensorState == 0)
    {
        status = VL53L1X_BootState(VL53_I2C_Address, &sensorState);
        HAL_Delay(2);
    }
      #ifdef debug
      UART_PutStr("Sensor booted\r\n");
      #endif
      
    // Set desired register settings
    status = VL53L1X_SensorInit(VL53_I2C_Address);
    status = VL53L1X_SetDistanceMode(VL53_I2C_Address, LONG_RANGE);           /* LONGE_RANGE or SHORT_RANGE */
    status = VL53L1X_SetTimingBudgetInMs(VL53_I2C_Address, 20);      /* in ms possible values [20, 50, 100, 200, 500] */
    status = VL53L1X_SetInterMeasurementInMs(VL53_I2C_Address, 20); /* in ms, IM must be > = TB */
//  status = VL53L1X_SetOffset(VL53_I2C_Address,20); /* offset compensation in mm */
    status = VL53L1X_SetROI(VL53_I2C_Address, 16, 16); /* minimum ROI 4,4 */
//	status = VL53L1X_CalibrateOffset(VL53_I2C_Address, 140, &offset); /* may take few second to perform the offset cal*/
//	status = VL53L1X_CalibrateXtalk(VL53_I2C_Address, 1000, &xtalk); /* may take few second to perform the xtalk cal */
      #ifdef debug
      UART_PutStr("Setup complete");
      #endif
}