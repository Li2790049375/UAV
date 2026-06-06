#ifndef __FILTER_H__
#define __FILTER_H__

#include "Com_debug.h"

// 卡尔曼滤波器结构体
typedef struct
{
    float LastP;    // 上一时刻的状态方差（或协方差）
    float Now_P;    // 当前时刻的状态方差（或协方差）
    float out;      // 滤波器的输出值，即估计的状态
    float Kk;       // 卡尔曼增益，用于调节预测值和测量值之间的权重
    float W;        // 过程噪声的方差，反映系统模型的不确定性
    float V;        // 测量噪声的方差，反映测量过程的不确定性
} KalmanFilter_Struct;

extern KalmanFilter_Struct kfs[3];
float Filter_LowPass(float newValue, float lastValue);

double Filter_Kalman(KalmanFilter_Struct *kf, double input);

#endif
