/************************************
 *       VL53 Main source file      *
 ************************************/

// Includes
#include "vl53_main.h"
#include "hardware_functions.h"

//Global variables
extern uint16_t PeopleCount;

// Programmable I2C address of the VL53L1X sensor
uint16_t VL53_I2C_Address = (0x29 << 1); // 7 bit, MSB

uint8_t buffer[50];
uint16_t wordData, Distance, AmbientRate, SpadNum, SignalRate, SensorID;
uint8_t status, byteData, sensorState = 0, RangeStatus, ToFSensor = 1, dataReady;

int VL53_Setup()
{
    // Wait for sensor to boot
    while (sensorState == 0)
    {
        status = VL53L1X_BootState(VL53_I2C_Address, &sensorState);
        HAL_Delay(2);
    }
    sensorState = 0;

// Print process, debug purposes
#ifdef debug
    UART_PutStr((uint8_t *)"Sensor booted\r\n");
#endif

    // Set desired register settings
    status = VL53L1X_SensorInit(VL53_I2C_Address);
    status += VL53L1X_SetDistanceMode(VL53_I2C_Address, LONG_RANGE);
    status += VL53L1X_SetTimingBudgetInMs(VL53_I2C_Address, SPEED_50_HZ);
    status += VL53L1X_SetInterMeasurementInMs(VL53_I2C_Address, SPEED_50_HZ);
    status += VL53L1X_SetROI(VL53_I2C_Address, 8, 16);

// Print process, debug purposes
#ifdef debug
    if (status == 0)
        UART_PutStr((uint8_t *)"Setup complete\r\n");
#endif

    // Of any error occured, terminate process
    if (status != 0)
    {
        UART_PutStr((uint8_t *)"Error in VL53 Ssetup function\n\r");
        return (-1);
    }
    return status;
}

// Starts measuring each zone to put in algorithm
int start_measuring()
{
    static int center[2] = SPAD_CENTERS; // These are the spad center of the 2 8x16 zones
    static int Zone = 0;                 // Index for center array

#ifdef debug
    static uint8_t start_done = 0;
    if (!start_done)
        UART_PutStr((uint8_t *)"Start Ranging\n");
    start_done = 1;
#endif

    // Wake up sensor
    status = VL53L1X_StartRanging(VL53_I2C_Address);

    // Read and display data
    while (1)
    {
        dataReady = 0; // Set dataReady to 0 before reading
        while (!dataReady)
        {
            status = VL53L1X_CheckForDataReady(VL53_I2C_Address, &dataReady);
            HAL_Delay(2);
        }
        status += VL53L1X_GetRangeStatus(VL53_I2C_Address, &RangeStatus);
        status += VL53L1X_GetDistance(VL53_I2C_Address, &Distance);
        status += VL53L1X_ClearInterrupt(VL53_I2C_Address); // Clear interrupt before next measurement is called

        // If any error occured, terminate process
        if (status != 0)
        {
            UART_PutStr((uint8_t *)"Error: could not read measurement\n\r");
            return (-1);
        }

        // Wait a few milliseconds to make sure measurung is done, then change the ROI
        HAL_Delay(5);
        status = VL53L1X_SetROICenter(VL53_I2C_Address, center[Zone]);

        // If any error occured, terminate process
        if (status != 0)
        {
            UART_PutStr((uint8_t *)"Error: could not set new zone\n\r");
            return (-1);
        }

        // Inject the measured distance in the people counting algorithm, return new number of people
        PeopleCount = CountingAlgorithm(Distance, Zone);

        // Toggle Zone integer between 0 and 1
        Zone++;
        Zone = Zone % 2;

#ifdef debug
        // Print number of people to a screen
        display_peoplecounter(PeopleCount);
#endif
    }
    return status;
}

int CountingAlgorithm(int16_t Distance, uint8_t zone)
{
    static int PathTrack[] = {0, 0, 0, 0};
    static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
    static int LeftPreviousStatus = NOBODY;
    static int RightPreviousStatus = NOBODY;
    static int PeopleCount = 0;

// Print process, debug purposes
#ifdef debug
    static uint8_t start_done = 0;
    if (!start_done)
        UART_PutStr((uint8_t *)"Start counting\n");
    start_done = 1;
#endif

    uint8_t CurrentZoneStatus = NOBODY;
    uint8_t AllZonesCurrentStatus = 0;
    uint8_t AnEventHasOccured = 0;

    if (Distance < DIST_THRESHOLD_MAX)
        CurrentZoneStatus = SOMEONE; // Someone is visible

    // Zone 0
    if (!zone)
    {
        if (CurrentZoneStatus != LeftPreviousStatus)
        {
            AnEventHasOccured = 1; // event in left zone has occured
            if (CurrentZoneStatus == SOMEONE)
                AllZonesCurrentStatus += 1;
            if (RightPreviousStatus == SOMEONE)     // need to check right zone as well ...
                AllZonesCurrentStatus += 2;         // event in left zone has occured
            LeftPreviousStatus = CurrentZoneStatus; // remember for next time
        }
    }
    // Zone 1
    else
    {
        if (CurrentZoneStatus != RightPreviousStatus)
        {
            AnEventHasOccured = 1; // event in left zone has occured
            if (CurrentZoneStatus == SOMEONE)
                AllZonesCurrentStatus += 2;
            if (LeftPreviousStatus == SOMEONE)       // need to left right zone as well ...
                AllZonesCurrentStatus += 1;          // event in left zone has occured
            RightPreviousStatus = CurrentZoneStatus; // remember for next time
        }
    }

    // if an event has occured
    if (AnEventHasOccured)
    {
        if (PathTrackFillingSize < 4)
            PathTrackFillingSize++;

        // if nobody anywhere lets check if an exit or entry has happened
        if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY))
        {
            // check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
            if (PathTrackFillingSize == 4)
            {
                // check exit or entry. no need to check PathTrack[0] == 0 , it is always the case
                if ((PathTrack[1] == 1) && (PathTrack[2] == 3) && (PathTrack[3] == 2))
                    PeopleCount++; // This an entry
                else if ((PathTrack[1] == 2) && (PathTrack[2] == 3) && (PathTrack[3] == 1))
                    PeopleCount--; // This an exit
            }
            PathTrackFillingSize = 1;
        }
        else
        {
            // update PathTrack
            // example of PathTrack update
            // 0
            // 0 1
            // 0 1 3
            // 0 1 3 1
            // 0 1 3 3
            // 0 1 3 2 ==> if next is 0 : check if exit
            PathTrack[PathTrackFillingSize - 1] = AllZonesCurrentStatus;
        }
    }
    return (PeopleCount);
}

#ifdef debug
// Function displays amount of people that entered the room
void display_peoplecounter(int peoplecounter)
{
    static int counter = 0;
    if (peoplecounter != counter)
    {
        sprintf((char *)buffer, "People Counter: %d\n", peoplecounter);
        UART_PutStr(buffer);
        counter = peoplecounter;
    }
}

// Function displays if either of the 'zones' see anything
void display_zones()
{
#define treshhold 1780
    static int center[2] = {167, 231}; /* these are the spad center of the 2 8*16 zones */
    static int zone[2] = {0, 0};
    static int Zonenr = 0;

    status = VL53L1X_StartRanging(VL53_I2C_Address);

    /* read and display data */
    while (1)
    {
        HAL_Delay(10);
        status = VL53L1X_SetROICenter(VL53_I2C_Address, center[Zonenr]);
        while (dataReady == 0)
        {
            status = VL53L1X_CheckForDataReady(VL53_I2C_Address, &dataReady);
            HAL_Delay(2);
        }
        dataReady = 0;
        status += VL53L1X_GetRangeStatus(VL53_I2C_Address, &RangeStatus);
        status += VL53L1X_GetDistance(VL53_I2C_Address, &Distance);
        status += VL53L1X_ClearInterrupt(VL53_I2C_Address); /* clear interrupt has to be called to enable next interrupt*/
        if (Distance < treshhold)
            zone[Zonenr] = 1;
        else
            zone[Zonenr] = 0;
        sprintf((char *)buffer, "Zone 1: %u, Zone 2: %u\n", zone[0], zone[1]);
        UART_PutStr(buffer);

        Zonenr++;
        Zonenr = Zonenr % 2;
    }
}
#endif
