#include "BUZZ.h"

void Buzz_On(void)
{
    BUZZ_GPIO_Port->BSRR = BUZZ_Pin;
}

void Buzz_Off(void)
{
    BUZZ_GPIO_Port->BRR = BUZZ_Pin;
}
