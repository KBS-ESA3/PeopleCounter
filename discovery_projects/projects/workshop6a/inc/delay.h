/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#ifndef _DELAY_H_
#define _DELAY_H_

#define DELAY_MS SystemCoreClock/8/1000
#define DELAY_S SystemCoreClock/8
// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void  Delay(const int d);

#endif /* _DELAY_H_ */
