#include "LED.h"

void LED_Start(LED_Struct *led)
{
    HAL_TIM_PWM_Start(led->tim, led->channel);
}


void LED_Set_PWM(LED_Struct *led)
{
    if(led->pwm > 1000)
    {
        debug_printf("灯光pwm数值错误\r\n");
        return;
    }
    __HAL_TIM_SET_COMPARE(led->tim, led->channel, led->pwm);
}

