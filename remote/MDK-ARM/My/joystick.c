#include "joystick.h"

volatile uint16_t adc_buf[4] = {0};

void Joystick_Init()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, 4);
}

void Joystick_GetValue(Joystick_Struct *joystick)
{
    joystick->yaw   = (int16_t)adc_buf[0];
    joystick->thr   = (int16_t)adc_buf[1];
    joystick->pitch = (int16_t)adc_buf[2];
    joystick->roll  = (int16_t)adc_buf[3];
}
