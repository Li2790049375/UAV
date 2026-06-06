#include "Filter.h"

#define     A       0.04f     // 一阶低通滤波系数

float Filter_LowPass(float newValue, float lastValue)
{
    return A * newValue + (1.0f - A) * lastValue;
}

/* 卡尔曼滤波 https://www.mwrf.net/tech/basic/2023/30081.html */

// 卡尔曼滤波参数
KalmanFilter_Struct kfs[3] = {
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543}};
double Filter_Kalman(KalmanFilter_Struct *kf, double input)
{
    kf->Now_P = kf->LastP + kf->W;
    kf->Kk    = kf->Now_P / (kf->Now_P + kf->V);
    kf->out   = kf->out + kf->Kk * (input - kf->out);
    kf->LastP = (1 - kf->Kk) * kf->Now_P;
    return kf->out;
}
