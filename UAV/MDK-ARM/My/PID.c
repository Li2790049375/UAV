#include "PID.h"

void PID_Calc(PID_Struct *pid)
{
    // 误差
    pid->Error = pid->Target - pid->measure;
    // 积分误差
    pid->ErrorInt += pid->Error;
    // 积分限幅
    if(pid->ErrorInt > 300.0f)  pid->ErrorInt = 300.0f;
    if(pid->ErrorInt < -300.0f) pid->ErrorInt = -300.0f;

    if(pid->Flag == 0)
    {
        pid->Last_Error = pid->Error;
        pid->Flag = 1;
    }

    // 微分误差
    pid->der = pid->Error - pid->Last_Error;
    // 输出
    pid->Output = (pid->Kp * pid->Error) + (pid->Ki * pid->ErrorInt * pid->period) + (pid->Kd * pid->der / pid->period);

    // 输出限幅:
    if(pid->Output >  400.0f) pid->Output =  400.0f;
    if(pid->Output < -400.0f) pid->Output = -400.0f;

    // 上一次误差
    pid->Last_Error = pid->Error;
}

void PID_Calc_Chain(PID_Struct *out_pid, PID_Struct *in_pid)
{
    // 外环
    PID_Calc(out_pid);
    // 外环的输出值作为内环的目标值
    in_pid->Target = out_pid->Output;
    // 内环
    PID_Calc(in_pid);
}

int16_t Motor_limit(int16_t speed, int16_t min_speed, int16_t max_speed)
{
    if(speed < min_speed)
    {
        return min_speed;
    }
    else if(speed > max_speed)
    {
        return max_speed;
    }
    else
    {
        return speed;
    }
}
