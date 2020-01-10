#include "error_handling.h"

void send_Warning(char *message)
{
   UART_PutStr(message);
}
