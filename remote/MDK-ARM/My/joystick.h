#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "adc.h"

typedef struct {

    int16_t yaw;
    int16_t thr;
    int16_t pitch;
    int16_t roll;

} Joystick_Struct;

void Joystick_Init(void);
void Joystick_GetValue(Joystick_Struct *joystick);

#endif 
