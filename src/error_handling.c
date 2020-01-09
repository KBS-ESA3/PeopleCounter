#include "error_handling.h"

void send_Warning(uint8_t *message)
{
   UART_PutStr(message);
}
