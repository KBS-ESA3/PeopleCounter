/************************************
 *        VL53 Main header file     *
 ************************************/
#ifndef __VL53_MAIN_H
#define __VL53_MAIN_H

// Uncomment the line below to enter debug mode
#define debug

// Includes
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

// Functions
int VL53_setup();
void VL53_get_distance();
int VL53_PeopleCount();
int ProcessPeopleCountingData(int16_t Distance, uint8_t zone);
void display_peoplecounter(int peoplecounter);

// Defines
#define NOBODY 0
#define SOMEONE 1
#define LEFT 0
#define RIGHT 1
#define SPEED_50_HZ 20 // 20ms time budget means 1/0.02 = 50 Hz

#define DIST_THRESHOLD_MAX  1780
#define LONG_RANGE 2 
#define SHORT_RANGE 1

#endif /* __VL53_MAIN_H */
