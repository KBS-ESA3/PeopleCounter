#include "error_handling.h"

void sendWarning(uint8_t* message)
{
   UART_PutStr(message);
}

