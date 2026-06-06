#ifndef __PID_H__
#define __PID_H__

#include "main.h"

typedef struct PID
{
    float Kp;
    float Ki;
    float Kd;

    float Last_Error;   // 上一次误差
    float Error;        // 当前误差
    float ErrorInt;     // 积分误差
    float der;          // 微分误差
    float Target;       // 目标值
    float measure;      // 测量值
    float Output;       // 输出结果
    float period;       // 计算周期

    uint8_t Flag;       // 首次标志位
}PID_Struct;

// 单环PID计算
void PID_Calc(PID_Struct *pid);
// 串级PID计算
void PID_Calc_Chain(PID_Struct *out_pid, PID_Struct *in_pid);
int16_t Motor_limit(int16_t speed, int16_t min_speed, int16_t max_speed);

#endif 
