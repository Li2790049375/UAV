#ifndef __LED_H__
#define __LED_H__

#include "main.h"
#include "Com_debug.h"

typedef struct {

    TIM_HandleTypeDef *tim;
    uint32_t channel;
    uint16_t pwm;

} LED_Struct;

void LED_Start(LED_Struct *led);
void LED_Set_PWM(LED_Struct *led);
#endif 
