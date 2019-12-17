/************************************
 *        VL53 Main header file     *
 ************************************/

#ifndef __VL53_MAIN_H
#define __VL53_MAIN_H

#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

void draw_SPAD_grid();
void VL53_setup();
void VL53_get_distance();

#define LONG_RANGE 2 
#define SHORT_RANGE 1

#endif /* __VL53_MAIN_H */

