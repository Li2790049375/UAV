#ifndef __FLIGHT_H
#define __FLIGHT_H  

#include "mpu6050.h"
#include "Com_debug.h"
#include "Filter.h"
#include "math.h"
#include "IMU.h"
#include "PID.h"
#include "Motor.h"
#include "My_LC08.h"
#include "Config.h"

extern float pitch_trim;
extern float roll_trim;
extern float yaw_trim;

void Flight_Init(void);
void Flight_Get_Euler_Angle(void);
void Flight_PID(const Remote_Data *rd);
void Flight_Motor(const Remote_Data *rd);
void Flight_Reset_PID(void);
void Flight_PID_Fix_Height(void);

#endif 
