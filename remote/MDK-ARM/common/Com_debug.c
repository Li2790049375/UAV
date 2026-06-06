#include "Com_debug.h"

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);

    return ch;
}
