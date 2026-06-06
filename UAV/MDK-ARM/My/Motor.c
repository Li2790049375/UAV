#include "Motor.h"

void Motor_Start(Motor_Struct *motor)
{
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, 0);
    HAL_TIM_PWM_Start(motor->tim, motor->channel);
}


void Motor_Set_Speed(Motor_Struct *motor)
{
    if(motor->speed > 1000 || motor->speed < 0)
    {
        debug_printf("电机速度不匹配\r\n");
        return;
    }
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, motor->speed);
}
