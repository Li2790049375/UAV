#ifndef __DATA_H__
#define __DATA_H__  

#include "Key.h"
#include "joystick.h"
#include "Com_debug.h"

typedef struct 
{
    int16_t yaw;
    int16_t thr;
    int16_t pitch;
    int16_t roll;
    uint8_t shutdown;
    uint8_t fix_height;
    uint8_t unlock;

}Remote_Data;

void Key_Data(void);
void Joystick_Data(void);

#endif 
