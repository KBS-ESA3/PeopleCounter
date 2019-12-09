/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void  Delay(const int d);
void  USART_Setup(void);
void  USART_Putstr(char *str);
void  USART_Clearscreen(void);
char *USART_itoa(int16_t i, char *p);

#endif /* _HELPER_H_ */
