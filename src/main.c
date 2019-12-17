#include "board_definitions.h"
#include "hardware_functions.h"
#include "error_handling.h"
#include "calipile_discovery.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

#define CALIPILE_0_ADDR (0x0C << 1)
#define CALIPILE_1_ADDR (0x0D << 1)

uint16_t VL53_I2C_Address = (0x29 << 1);

Calipile sensor0 = {CALIPILE_0_ADDR, 11, 8, 10, 50, 10, 10, 0x00, 0x08, 0x01, 0x00, 0, 0};
Calipile sensor1 = {CALIPILE_1_ADDR, 11, 8, 10, 50, 10, 10, 0x00, 0x08, 0x01, 0x00, 0, 0};

int main(void)
{
  char buffer[50];
  uint16_t wordData, Distance, AmbientRate, SpadNum, SignalRate;
  uint8_t status, byteData, sensorState = 0, dataReady, RangeStatus, ToFSensor = 1;

  HAL_Init();
  initLeds();
  UART_Init();
  I2C_Init();
  calipile_init(&sensor0);
  UART_clearScreen();
  print("VL53!\n\r");

  //VL53!!
  status = VL53L1_RdByte(VL53_I2C_Address, VL53L1_IDENTIFICATION__MODEL_ID, &byteData);
  sprintf(buffer, "%x\n\r", byteData);
  print(buffer);
  status = VL53L1_RdByte(VL53_I2C_Address, 0x0110, &byteData);
  sprintf(buffer, "%x\n\r", byteData);
  print(buffer);
  status = VL53L1_RdWord(VL53_I2C_Address, VL53L1_IDENTIFICATION__MODEL_ID, &wordData);
  sprintf(buffer, "%x\n\r", byteData);
  print(buffer);
  status = VL53L1_RdByte(VL53_I2C_Address, 0x0001, &byteData);
  sprintf(buffer, "%x\n\r", byteData);
  print(buffer);
  while (sensorState == 0)
  {
    status = VL53L1X_BootState(VL53_I2C_Address, &sensorState);
    HAL_Delay(2);
  }
  print("Chip booted\n\r");
  //END VL53!!
  while (1)
  {
    status = VL53L1X_SensorInit(VL53_I2C_Address);
    /* Optional functions to be used to change the main ranging parameters according the application requirements to get the best ranging performances */
    status = VL53L1X_SetDistanceMode(VL53_I2C_Address, 2);           /* 1=short, 2=long */
    status = VL53L1X_SetTimingBudgetInMs(VL53_I2C_Address, 100);     /* in ms possible values [20, 50, 100, 200, 500] */
    status = VL53L1X_SetInterMeasurementInMs(VL53_I2C_Address, 100); /* in ms, IM must be > = TB */
  //  status = VL53L1X_SetOffset(VL53_I2C_Address,20); /* offset compensation in mm */
  //  status = VL53L1X_SetROI(VL53_I2C_Address, 16, 16); /* minimum ROI 4,4 */
  //	status = VL53L1X_CalibrateOffset(VL53_I2C_Address, 140, &offset); /* may take few second to perform the offset cal*/
  //	status = VL53L1X_CalibrateXtalk(VL53_I2C_Address, 1000, &xtalk); /* may take few second to perform the xtalk cal */
    printf("VL53L1X Ultra Lite Driver Example running ...\n\r");
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
      sprintf(buffer, "%u, %u, %u, %u, %u\n\r", RangeStatus, Distance, SignalRate, AmbientRate, SpadNum);
      UART_PutStr(buffer);
    }
  }
}

  void SysTick_Handler(void)
  {
    HAL_IncTick();
  }

  void NMI_Handler(void)
  {
  }

  void HardFault_Handler(void)
  {
    while (1)
    {
    }
  }

  void MemManage_Handler(void)
  {
    while (1)
    {
    }
  }

  void BusFault_Handler(void)
  {
    while (1)
    {
    }
  }

  void UsageFault_Handler(void)
  {
    while (1)
    {
    }
  }

  void SVC_Handler(void)
  {
  }

  void DebugMon_Handler(void)
  {
  }

  void PendSV_Handler(void)
  {
  }
