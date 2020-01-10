/************************************
 *        VL53 Main header file     *
 ************************************/

#ifndef __VL53_MAIN_H
#define __VL53_MAIN_H

// !!! Uncomment the line below to enter debug mode !!!
#define debug

// Includes
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

// Global functions
int VL53_Setup();
int start_measuring();
uint16_t Get_PeopleCount();

#endif /* __VL53_MAIN_H */
