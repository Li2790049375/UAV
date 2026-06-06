#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "tim.h"
#include "Com_debug.h"

typedef struct {

    TIM_HandleTypeDef *tim;
    uint16_t channel;
    int16_t speed;

} Motor_Struct;

void Motor_Start(Motor_Struct *motor);
void Motor_Set_Speed(Motor_Struct *motor);

#endif 
