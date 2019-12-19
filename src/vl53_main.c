/************************************
 *       VL53 Main source file      *
 ************************************/

// Includes
#include "stm32f0xx_hal.h"
#include "vl53_main.h"
#include "hardware_functions.h"

// Programmabe I2C address of the VL53L1X sensor
uint16_t VL53_I2C_Address = (0x29 << 1);

uint8_t buffer[50];
uint16_t wordData, Distance, AmbientRate, SpadNum, SignalRate, SensorID;
uint8_t status, byteData, sensorState = 0, dataReady, RangeStatus, ToFSensor = 1;

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
      sprintf((char *)buffer, "%u, %u, %u, %u, %u\n\r", RangeStatus, Distance, SignalRate, AmbientRate, SpadNum);
      UART_PutStr(buffer);
      #endif
  }
}

int VL53_setup()
{
  // Wait for sensor to boot
  while (sensorState == 0)
  {
    status = VL53L1X_BootState(VL53_I2C_Address, &sensorState);
    HAL_Delay(2);
  }
  sensorState = 0;
    #ifdef debug
    UART_PutStr((uint8_t *)"Sensor booted\r\n");
    #endif
    
  // Set desired register settings
  status = VL53L1X_SensorInit(VL53_I2C_Address);
  status += VL53L1X_SetDistanceMode(VL53_I2C_Address, LONG_RANGE);           /* LONGE_RANGE or SHORT_RANGE */
  status += VL53L1X_SetTimingBudgetInMs(VL53_I2C_Address, SPEED_50_HZ);      /* in ms possible values [20, 50, 100, 200, 500] */
  status += VL53L1X_SetInterMeasurementInMs(VL53_I2C_Address, SPEED_50_HZ); /* in ms, IM must be > = TB */
//status += VL53L1X_SetDistanceThreshold(VL53_I2C_Address, 200, 200, 1, 1);
//status += VL53L1X_SetOffset(VL53_I2C_Address,20); /* offset compensation in mm */
  status += VL53L1X_SetROI(VL53_I2C_Address, 8, 16); /* minimum ROI 4,4 */
//status += VL53L1X_CalibrateOffset(VL53_I2C_Address, 140, &offset); /* may take few second to perform the offset cal*/
//status += VL53L1X_CalibrateXtalk(VL53_I2C_Address, 1000, &xtalk); /* may take few second to perform the xtalk cal */
    
    #ifdef debug
    if (status == 0) UART_PutStr((uint8_t *)"Setup complete\r\n");
    #endif
  if (status != 0) 
  {
    UART_PutStr((uint8_t *)"Error in VL53 Ssetup function\n\r");
    return (-1);
  }
  return status;
} 

int VL53_PeopleCount()
{
  static int center[2] = {167,231}; /* these are the spad center of the 2 8*16 zones */
  static int Zone = 0;
  static int PplCounter = 0;

  #ifdef debug
  static uint8_t start_done = 0;
  if (!start_done) UART_PutStr((uint8_t *)"Start Ranging\n");
  start_done = 1;
  #endif

  status = VL53L1X_SetROI(VL53_I2C_Address, 8, 16);
  status += VL53L1X_StartRanging(VL53_I2C_Address);

  /* read and display data */
  while(1) 
  { 
    while (dataReady == 0) {
    status = VL53L1X_CheckForDataReady(VL53_I2C_Address, &dataReady);
    HAL_Delay(2);
    }
    dataReady = 0;
    status += VL53L1X_GetRangeStatus(VL53_I2C_Address, &RangeStatus);
    status += VL53L1X_GetDistance(VL53_I2C_Address, &Distance);
    status += VL53L1X_ClearInterrupt(VL53_I2C_Address); /* clear interrupt has to be called to enable next interrupt*/
    
    if (status != 0) {
      UART_PutStr((uint8_t *)"Error in operating the VL53_I2C_Addressice\n\r");
      return (-1);
    }

    // wait a couple of milliseconds to ensure the setting of the new ROI center for the next ranging is effective
    // otherwise there is a risk that this setting is applied to current ranging (even if timing has expired, the intermeasurement
    // may expire immediately after.
    HAL_Delay(10);
    status = VL53L1X_SetROICenter(VL53_I2C_Address, center[Zone]);
    if (status != 0) 
    {
      UART_PutStr((uint8_t *)"Error in chaning the center of the ROI\n\r");
      return (-1);
    }
    // inject the new ranged distance in the people counting algorithm
    PplCounter = ProcessPeopleCountingData(Distance, Zone);

    Zone++;
    Zone = Zone%2;

    display_peoplecounter(PplCounter);

    // sprintf(buffer, "%d, %d, %d, %d\n\r", Zone, RangeStatus, Distance, PplCounter);
    // UART_PutStr(buffer);
    // if (status != 0) 
    // {
    //   UART_PutStr("Error in People Counter main function\n\r");
    //   return (-1);
    // }
  }
  return status;
}

int ProcessPeopleCountingData(int16_t Distance, uint8_t zone) 
{
  static int PathTrack[] = {0,0,0,0};
  static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
  static int LeftPreviousStatus = NOBODY;
  static int RightPreviousStatus = NOBODY;
  static int PeopleCount = 0;

  #ifdef debug
  static uint8_t start_done = 0;
  if(!start_done) UART_PutStr((uint8_t *)"Start counting\n");
  start_done = 1;
  #endif

  int CurrentZoneStatus = NOBODY;
  int AllZonesCurrentStatus = 0;
  int AnEventHasOccured = 0;

	if (Distance < DIST_THRESHOLD_MAX) {
		// Someone is in !
		CurrentZoneStatus = SOMEONE;
	}

	// left zone
	if (zone == LEFT) {

		if (CurrentZoneStatus != LeftPreviousStatus) {
			// event in left zone has occured
			AnEventHasOccured = 1;

			if (CurrentZoneStatus == SOMEONE) {
				AllZonesCurrentStatus += 1;
			}
			// need to check right zone as well ...
			if (RightPreviousStatus == SOMEONE) {
				// event in left zone has occured
				AllZonesCurrentStatus += 2;
			}
			// remember for next time
			LeftPreviousStatus = CurrentZoneStatus;
		}
	}
	// right zone
	else {
		if (CurrentZoneStatus != RightPreviousStatus) {

			// event in left zone has occured
			AnEventHasOccured = 1;
			if (CurrentZoneStatus == SOMEONE) {
				AllZonesCurrentStatus += 2;
			}
			// need to left right zone as well ...
			if (LeftPreviousStatus == SOMEONE) {
				// event in left zone has occured
				AllZonesCurrentStatus += 1;
			}
			// remember for next time
			RightPreviousStatus = CurrentZoneStatus;
		}
	}

	// if an event has occured
	if (AnEventHasOccured) {
		if (PathTrackFillingSize < 4) {
			PathTrackFillingSize ++;
		}

		// if nobody anywhere lets check if an exit or entry has happened
		if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY)) {

			// check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
			if (PathTrackFillingSize == 4) {
				// check exit or entry. no need to check PathTrack[0] == 0 , it is always the case

				if ((PathTrack[1] == 1)  && (PathTrack[2] == 3) && (PathTrack[3] == 2)) {
					// This an entry
					PeopleCount ++;

				} else if ((PathTrack[1] == 2)  && (PathTrack[2] == 3) && (PathTrack[3] == 1)) {
					// This an exit
					PeopleCount --;
				}
			}

			PathTrackFillingSize = 1;
		}
		else {
			// update PathTrack
			// example of PathTrack update
			// 0
			// 0 1
			// 0 1 3
			// 0 1 3 1
			// 0 1 3 3
			// 0 1 3 2 ==> if next is 0 : check if exit
			PathTrack[PathTrackFillingSize-1] = AllZonesCurrentStatus;
		}
	}
	// output debug data to main host machine
	return(PeopleCount);
}

void display_peoplecounter(int peoplecounter)
{
  static int counter = 0;
  if (peoplecounter != counter)
  {
    // Uncomment this line to use on PuTTY (or similar software)
    //UART_clearScreen();

    //Uncomment this line to use on serial monitor on smartphone
    //UART_PutStr("\r\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    sprintf((char *)buffer, "People Counter: %d\n", peoplecounter);
    UART_PutStr(buffer);
    counter = peoplecounter;
  }
}
