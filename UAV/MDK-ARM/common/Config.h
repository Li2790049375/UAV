#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "main.h"

typedef enum 
{
    REMOTE_DISCONNECTED,
    REMOTE_CONNECTED_N,
    REMOTE_CONNECTED_Y,
}Remote_State;

typedef enum
{  
    IDLE = 0,
    NORMAL,
    FIX_HEIGHT,
    FAIL,
}Flight_State;

typedef enum 
{
    FREE,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK
}Thr_State;

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

typedef struct
{
    int16_t gyro_x;     // 横滚角
    int16_t gyro_y;     // 俯仰角
    int16_t gyro_z;     // 偏航角
}Gyro_Struct;

typedef struct
{
    int16_t accel_x;    
    int16_t accel_y;    
    int16_t accel_z;    
}Accel_Struct;

typedef struct
{
    Gyro_Struct gyro;
    Accel_Struct accel;
}Gyro_Accel_Struct;

typedef struct 
{
    float yaw;
    float pitch;
    float roll;
}Euler_Struct;

#endif 
