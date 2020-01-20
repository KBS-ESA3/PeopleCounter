/************************************
 *        VL53 Main header file     *
 ************************************/

#ifndef __VL53_MAIN_H
#define __VL53_MAIN_H

// !!! Uncomment the line below to enter DEBUG mode !!!
#define DEBUG

// Includes
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

// Global functions
void VL53_setup();
void VL53_start_measuring();
uint16_t VL53_get_people_count();
void VL53_keep_measuring();

#endif /* __VL53_MAIN_H */
