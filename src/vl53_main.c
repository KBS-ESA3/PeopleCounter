// Includes
#include "vl53_main.h"
#include "hardware_functions.h"
#include "packet.h"
#include "network.h"

// Defines
#define SPEED_50_HZ 20              // 20ms time budget, means 1/0.02 = 50 Hz
#define SPAD_CENTERS {167, 231}     // Zone 0 and zone 1, 
#define NOBODY 0                    // Boolean, sensor does not see anyone
#define SOMEONE 1                   // Boolean, sensor sees someone
#define DIST_THRESHOLD_MAX  1780    // Distance from where the sensor sees an object in mm, measured from top down (of the doorframe)
#define SHORT_RANGE 1               // Up to 1300 mm
#define LONG_RANGE 2                // Up to 3600 mm
#define MSB ((uint32_t)(1<<15))     // MSB in a 16 bit value
#define VL53_ADDRESS (0x29 << 1)    // Initial VL53 address, 7 bit, MSB

// Global variables
int16_t people_count = 0;            // Should never be less than 0
extern uint8_t vl53_enable;
// Programmable I2C address of the VL53L1X sensor
uint8_t VL53_I2C_address = VL53_ADDRESS;   // 7 bit, MSB

// Local variables
static uint16_t distance = 0, last_people_count = 0;
static uint8_t status = 0, sensor_state = 0, range_status = 0;

// Local functions
int16_t VL53_counting_algorithm(uint16_t distance, uint8_t zone);
int16_t VL53_check_people_count(uint16_t people_count);

#ifdef DEBUG            // Only declare these functions if DEBUG mode is active
void VL53_display_people_counter(uint16_t people_count);
void VL53_display_zones();   // Zones are the areas the sensor reads from 
#endif

// Function initializes VL53 sensor with I2C 
void VL53_setup()
{
    #ifdef DEBUG
    UART_PutStr((uint8_t *)"Booting sensor...\r\n");
    #endif
    // Wait for sensor to boot
    while (!sensor_state)
    {
        status = VL53L1X_BootState(VL53_I2C_address, &sensor_state);
        HAL_Delay(2);
    }
    sensor_state = 0;

// Print process, debug purposes
#ifdef DEBUG
    UART_PutStr((uint8_t *)"Sensor booted\r\n");
#endif

    // Set desired register settings
    status = VL53L1X_SensorInit(VL53_I2C_address);
    status += VL53L1X_SetDistanceMode(VL53_I2C_address, LONG_RANGE);
    status += VL53L1X_SetTimingBudgetInMs(VL53_I2C_address, SPEED_50_HZ);
    status += VL53L1X_SetInterMeasurementInMs(VL53_I2C_address, SPEED_50_HZ);
    status += VL53L1X_SetROI(VL53_I2C_address, 8, 16);

// Print process, debug purposes
#ifdef DEBUG
    if (!status)
        UART_PutStr((uint8_t *)"Setup complete\r\n");
#endif

    // Of any error occured, terminate process
    if (status)
    {   
        ERROR_send_warning("Error in VL53 Setup function\r\n");
        return;
    }
}

// Starts measuring each zone to put in algorithm
// Wakes up sensor, and starts ranging (in while loop), injecting data in algorithm
void VL53_start_measuring()
{
    static int zone_center[2] = SPAD_CENTERS; // These are the spad center of the 2 8x16 zones
    static int zone = 0;                 // Index for center array
    static uint8_t data_ready = 0;

#ifdef DEBUG
    static uint8_t start_done = 0;
    if (!start_done)
    {
        UART_PutStr((uint8_t *)"Start Ranging\r\n");
    }
    start_done = 1;
#endif

    // Wake up sensor
    status = VL53L1X_StartRanging(VL53_I2C_address);

    // While loop is needed, continuous 
    while (vl53_enable)
    {
        data_ready = 0; // Set data_ready to 0 before reading
        while (!data_ready)
        {
            status = VL53L1X_CheckForDataReady(VL53_I2C_address, &data_ready);
        }
        status += VL53L1X_GetRangeStatus(VL53_I2C_address, &range_status);
        status += VL53L1X_GetDistance(VL53_I2C_address, &distance);
        status += VL53L1X_ClearInterrupt(VL53_I2C_address); // Clear interrupt before next measurement is called

        // If any error occured, terminate process
        if (status)
        {   
            ERROR_send_warning("Error: could not read measurement\r\n");
            return;
        }

        // Wait a few milliseconds to make sure measurung is done, then change the ROI
        HAL_Delay(5);
        status = VL53L1X_SetROICenter(VL53_I2C_address, zone_center[zone]);

        // If any error occured, terminate process
        if (status)
        {
            ERROR_send_warning("Error: could not set new zone\r\n");
            return;
        }

        // Inject the measured distance in the people counting algorithm, return new number of people
        people_count = VL53_counting_algorithm(distance, zone);
        // Check people_count for faulty value (i.e. when the value becomes negative)
        people_count = VL53_check_people_count(people_count);

        // Toggle zone integer between 0 and 1
        zone++;
        zone = zone % 2;
    }
}

// Function calculates the amount of people in a room, needs to be called continuously to work
int16_t VL53_counting_algorithm(uint16_t distance, uint8_t zone)
{
    static uint8_t path_track[] = {0, 0, 0, 0};
    static uint8_t path_track_filling_size = 1;        // Init this to 1 as we start from state where nobody is any of the zones
    static uint8_t left_previous_status = NOBODY;
    static uint8_t right_previous_status = NOBODY;

// Print process, debug purposes
#ifdef DEBUG
    static uint8_t start_done = 0;
    if (!start_done)
    {
        UART_PutStr((uint8_t *)"Start counting\r\n");
    }
    start_done = 1;
#endif

    uint8_t current_zone_status = NOBODY;
    uint8_t all_zones_current_status = 0;
    uint8_t event_occured = 0;

    if (distance < DIST_THRESHOLD_MAX)
    {
        current_zone_status = SOMEONE; // Someone is visible
    }

    if (!zone)  // Zone 0
    {
        if (current_zone_status != left_previous_status)
        {
            event_occured = 1; // Event in left zone has occured
            if (current_zone_status == SOMEONE)
            {
                all_zones_current_status += 1;
            }
            if (right_previous_status == SOMEONE)     // Need to check right zone as well ...
            {
                all_zones_current_status += 2;         // Event in left zone has occured
            } 
            left_previous_status = current_zone_status; // Remember for next time
        }
    }
    else    // Zone 1
    {
        if (current_zone_status != right_previous_status)
        {
            event_occured = 1; // Event in left zone has occured
            if (current_zone_status == SOMEONE)
            {
                all_zones_current_status += 2;
            }  
            if (left_previous_status == SOMEONE)       // Need to left right zone as well ...
            {
                all_zones_current_status += 1;          // Event in left zone has occured
            }
            right_previous_status = current_zone_status; // Remember for next time
        }
    }
    if (event_occured)
    {
        if (path_track_filling_size < 4)
        {
            path_track_filling_size++;
        }

        // If nobody anywhere lets check if an exit or entry has happened
        if ((left_previous_status == NOBODY) && (right_previous_status == NOBODY))
        {
            // Check exit or entry only if path_track_filling_size is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
            if (path_track_filling_size == 4)
            {
                // Check exit or entry. No need to check path_track[0] == 0 , it is always the case
                if ((path_track[1] == 1) && (path_track[2] == 3) && (path_track[3] == 2))
                {
                    people_count++; // This an entry
                    on_passing();
                }
                else if ((path_track[1] == 2) && (path_track[2] == 3) && (path_track[3] == 1))
                {
                    people_count--; // This an exit
                    on_passing();    
                }
            }
            path_track_filling_size = 1;
        }
        else
        {
            // update path_track
            // example of path_track update
            // 0
            // 0 1
            // 0 1 3
            // 0 1 3 1
            // 0 1 3 3
            // 0 1 3 2 ==> if next is 0 : check if exit
            path_track[path_track_filling_size - 1] = all_zones_current_status;
        }
    }
    return people_count;
}

// Function checks if the people_count value is not faulty
int16_t VL53_check_people_count(uint16_t people_count)
{
    if (people_count & MSB) // In Two's complement, MSB is set and the other bits are flipped when value is negative. Check if MSB is set.
    {
        set_Led(LED1, LED_ON); // Turn on green error led
        if (people_count != last_people_count)
        {   
            ERROR_send_warning("Negative Value: Error in count!\r\n");
            people_count = last_people_count = 0;
            ERROR_send_warning("People Counter set to 0!\r\n");
        }
    }
    return people_count;
}

// Function to return the People Count
uint16_t VL53_get_people_count()
{
    return people_count;
}


// These functions are only needed in DEBUG mode
#ifdef DEBUG

// Function displays amount of people that entered the room
void VL53_display_people_counter(uint16_t people_count)
{
    char buffer[50];
    if (people_count != last_people_count)
    {
        sprintf((char *)buffer, "People Counter: %d\r\n", people_count);
        UART_PutStr(buffer);
        last_people_count = people_count;
    }
}

// Function displays if either of the 'zones' see anything
void VL53_display_zones()
{
    char buffer[50];
    const int center[2] = {167, 231}; /* these are the spad center of the 2 8*16 zones */
    static int zone[2] = {0, 0};
    static int zone_index = 0;
    static uint8_t data_ready = 0;

    status = VL53L1X_StartRanging(VL53_I2C_address);

    // Read and display data, similar to start_measuring()
    while (1)
    {
        HAL_Delay(10);  // Print on a slight delay
        status = VL53L1X_SetROICenter(VL53_I2C_address, center[zone_index]);
        while (data_ready == 0)
        {
            status = VL53L1X_CheckForDataReady(VL53_I2C_address, &data_ready);
            HAL_Delay(2);
        }
        data_ready = 0;
        status += VL53L1X_GetRangeStatus(VL53_I2C_address, &range_status);
        status += VL53L1X_GetDistance(VL53_I2C_address, &distance);
        status += VL53L1X_ClearInterrupt(VL53_I2C_address);
        if (distance < DIST_THRESHOLD_MAX)
        {
            zone[zone_index] = 1;
        }
        else 
        {
            zone[zone_index] = 0;
        }
        sprintf((char *)buffer, "Zone 1: %u, Zone 2: %u\r\n", zone[0], zone[1]);
        UART_PutStr(buffer);

        zone_index++;
        zone_index = zone_index % 2;
    }
}
#endif // End DEBUG 
