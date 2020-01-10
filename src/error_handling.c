#include "error_handling.h"

void ERROR_send_warning(char *message)
{
   UART_PutStr(message);
}
