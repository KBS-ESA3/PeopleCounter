/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#include "helper.h"

/**
  * @brief  This function implements a delay.
  *         If the optimization level is set to -O3, this function takes 8
  *         cycles. To create a delay of 1 second, use the following function
  *         call: Delay(SystemCoreClock/8);
  * @param  d: number delay loops (1 loop takes 8/SystemCoreClock sec.)
  * @retval None
  */
#pragma push
#pragma O3
void Delay(const int d)
{
  volatile int i;

  for(i=d; i>0; i--){ ; }

  return;
}
#pragma pop
