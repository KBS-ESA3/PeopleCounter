#include "error_handling.h"

void sendWarning(char *message)
{
    UART_PutStr(message);
}
