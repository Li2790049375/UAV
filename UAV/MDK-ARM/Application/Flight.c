#include "Flight.h"

Gyro_Accel_Struct gyro_accel_data = {0};
Euler_Struct euler_angle = {0};
Gyro_Struct last_gyro = {0};
double gyro_z_sum;
float pitch_trim = 0.0f;
float roll_trim = 0.0f;
float yaw_trim = 0.0f;

extern Remote_Data remote_data;
extern Flight_State flight_state;
extern uint16_t fix_height;
extern TaskHandle_t com_handle;

#define REMOTE_MID_VALUE       500
#define REMOTE_DEADBAND        20
#define REMOTE_ANGLE_SCALE     50.0f
#define PID_ENABLE_THR         10

// 电机结构体
Motor_Struct motor1 = {.tim = &htim2, .channel = TIM_CHANNEL_1, .speed = 0};
Motor_Struct motor2 = {.tim = &htim2, .channel = TIM_CHANNEL_2, .speed = 0};
Motor_Struct motor3 = {.tim = &htim2, .channel = TIM_CHANNEL_3, .speed = 0};
Motor_Struct motor4 = {.tim = &htim2, .channel = TIM_CHANNEL_4, .speed = 0};

// 俯仰角
PID_Struct pitch_pid = {.Kp = 8.0, .Ki = 0, .Kd = 0, .Flag = 0, .period = 0.006f};     // 外环 
PID_Struct gyro_y_pid = {.Kp = 3, .Ki = 0.15, .Kd = 0.4, .Flag = 0, .period = 0.006f};    // 内环
// PID_Struct pitch_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};     // 外环 
// PID_Struct gyro_y_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};    // 内环

// 横滚角
PID_Struct roll_pid = {.Kp = 8.0, .Ki = 0, .Kd = 0, .Flag = 0, .period = 0.006f};     // 外环 
PID_Struct gyro_x_pid = {.Kp = 3, .Ki = 0.2, .Kd = 0.4, .Flag = 0, .period = 0.006f};    // 内环
// PID_Struct roll_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};     // 外环 
// PID_Struct gyro_x_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};    // 内环

// 偏航角
PID_Struct yaw_pid = {.Kp = 6, .Ki = 0, .Kd = 0, .Flag = 0, .period = 0.006f};     
PID_Struct gyro_z_pid = {.Kp = 8, .Ki = 0, .Kd = 0.8, .Flag = 0, .period = 0.006f};
// PID_Struct yaw_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};     
// PID_Struct gyro_z_pid = {.Kp = 0, .Ki = 0, .Kd = 0, .Flag = 0};

// 定高（周期 4×6ms = 24ms）
PID_Struct fix_height_pid = {.Kp = 0.6, .Ki = 0, .Kd = 0.2, .Flag = 0, .period = 0.024f};


void Flight_Reset_PID(void)
{
    gyro_z_sum = 0;

    memset(&pitch_pid, 0, sizeof(pitch_pid));
    memset(&gyro_y_pid, 0, sizeof(gyro_y_pid));
    memset(&roll_pid, 0, sizeof(roll_pid));
    memset(&gyro_x_pid, 0, sizeof(gyro_x_pid));
    memset(&yaw_pid, 0, sizeof(yaw_pid));
    memset(&gyro_z_pid, 0, sizeof(gyro_z_pid));
    memset(&fix_height_pid, 0, sizeof(fix_height_pid));

    pitch_pid.Kp = 8.0;  pitch_pid.Ki = 0; pitch_pid.Kd = 0; pitch_pid.period = 0.006f;
    gyro_y_pid.Kp = 3; gyro_y_pid.Ki = 0.15; gyro_y_pid.Kd = 0.4; gyro_y_pid.period = 0.006f;
    roll_pid.Kp = 8.0;   roll_pid.Ki = 0;  roll_pid.Kd = 0; roll_pid.period = 0.006f;
    gyro_x_pid.Kp = 3; gyro_x_pid.Ki = 0.2; gyro_x_pid.Kd = 0.4; gyro_x_pid.period = 0.006f;
    yaw_pid.Kp = 6;     yaw_pid.Ki = 0;   yaw_pid.Kd = 0; yaw_pid.period = 0.006f;
    gyro_z_pid.Kp = 8; gyro_z_pid.Ki = 0; gyro_z_pid.Kd = 0.8; gyro_z_pid.period = 0.006f;
    fix_height_pid.Kp = 0.6; fix_height_pid.Ki = 0; fix_height_pid.Kd = 0.2; fix_height_pid.period = 0.024f;
}

static float Flight_Remote_To_Target(int16_t value)
{
    int16_t offset = value - REMOTE_MID_VALUE;

    if(offset > -REMOTE_DEADBAND && offset < REMOTE_DEADBAND)
    {
        offset = 0;
    }

    return offset / REMOTE_ANGLE_SCALE;
}

void Flight_Init()
{
    MPU6050_Init();
    
    Motor_Start(&motor1);
    Motor_Start(&motor2);
    Motor_Start(&motor3);
    Motor_Start(&motor4);   

    My_LC08_Init();
}

void Flight_Get_Euler_Angle()
{   
    MPU6050_Get_Data(&gyro_accel_data);

// 低通滤波
    gyro_accel_data.gyro.gyro_x = Filter_LowPass(gyro_accel_data.gyro.gyro_x, last_gyro.gyro_x);
    gyro_accel_data.gyro.gyro_y = Filter_LowPass(gyro_accel_data.gyro.gyro_y, last_gyro.gyro_y);
    gyro_accel_data.gyro.gyro_z = Filter_LowPass(gyro_accel_data.gyro.gyro_z, last_gyro.gyro_z);

    last_gyro.gyro_x = gyro_accel_data.gyro.gyro_x;
    last_gyro.gyro_y = gyro_accel_data.gyro.gyro_y;
    last_gyro.gyro_z = gyro_accel_data.gyro.gyro_z;

    // 打印角速度
    // debug_printf(":%d,%d,%d\n", gyro_accel_data.gyro.gyro_x, gyro_accel_data.gyro.gyro_y, gyro_accel_data.gyro.gyro_z);

    // kalman滤波
    gyro_accel_data.accel.accel_x = Filter_Kalman(&kfs[0], gyro_accel_data.accel.accel_x);
    gyro_accel_data.accel.accel_y = Filter_Kalman(&kfs[1], gyro_accel_data.accel.accel_y);
    gyro_accel_data.accel.accel_z = Filter_Kalman(&kfs[2], gyro_accel_data.accel.accel_z);

    // 打印加速度
    // debug_printf(":%d,%d,%d\n", gyro_accel_data.accel.accel_x, gyro_accel_data.accel.accel_y, gyro_accel_data.accel.accel_z);



// // 互补解算计算欧拉角(加速度)(俯仰角，横滚角)
    // euler_angle.pitch = atan2(gyro_accel_data.accel.accel_x, gyro_accel_data.accel.accel_z) / 3.1415926 * 180.0;
    // euler_angle.roll  = atan2(gyro_accel_data.accel.accel_y, gyro_accel_data.accel.accel_z) / 3.1415926 * 180.0;

// // 角速度积分(偏航角)
    // gyro_z_sum += (gyro_accel_data.gyro.gyro_z * 2000.0f / 32768.0f) * 0.006;
    // euler_angle.yaw = gyro_z_sum;



// 四元数姿态解算
    Common_IMU_GetEulerAngle(&gyro_accel_data, &euler_angle, 0.006);

    // debug_printf(":%.2f,%.2f,%.2f\n", euler_angle.pitch, euler_angle.roll, euler_angle.yaw);
}

void Flight_PID(const Remote_Data *rd)
{
    if(flight_state == IDLE || flight_state == FAIL || rd->thr <= PID_ENABLE_THR)
    {
        Flight_Reset_PID();
        return;
    }

// 俯仰角
// remote_data.pitch(0~1000,500为中心点)控制到±50°
    pitch_pid.Target = -Flight_Remote_To_Target(rd->pitch);
    // 外环
    pitch_pid.measure = euler_angle.pitch - pitch_trim;
    // 内环
    gyro_y_pid.measure = gyro_accel_data.gyro.gyro_y * 2000.0f / 32768.0f;

    PID_Calc_Chain(&pitch_pid, &gyro_y_pid);

    // debug_printf(":%.2f,%.2f\n", gyro_y_pid.Error, gyro_y_pid.Output);

// 横滚角
// remote_data.roll(0~1000,500为中心点)控制到±50°
    roll_pid.Target = -Flight_Remote_To_Target(rd->roll);
    // 外环
    roll_pid.measure = euler_angle.roll - roll_trim;
    // 内环
    gyro_x_pid.measure = gyro_accel_data.gyro.gyro_x * 2000.0f / 32768.0f;

    PID_Calc_Chain(&roll_pid, &gyro_x_pid);

// 偏航角
    yaw_pid.Target = -Flight_Remote_To_Target(rd->yaw);
    // 外环
    yaw_pid.measure = euler_angle.yaw - yaw_trim;
    // 内环
    gyro_z_pid.measure = gyro_accel_data.gyro.gyro_z * 2000.0f / 32768.0f;

    PID_Calc_Chain(&yaw_pid, &gyro_z_pid);
}

void Flight_Motor(const Remote_Data *rd)
{
    switch (flight_state)
    {
    case IDLE:
        motor1.speed = 0;
        motor2.speed = 0;
        motor3.speed = 0;
        motor4.speed = 0;
        break;
    case NORMAL:
        if(rd->thr <= PID_ENABLE_THR)
        {
            motor1.speed = rd->thr;
            motor2.speed = rd->thr;
            motor3.speed = rd->thr;
            motor4.speed = rd->thr;
        }
        else
        {
            motor1.speed = rd->thr + gyro_y_pid.Output + gyro_x_pid.Output + gyro_z_pid.Output;
            motor2.speed = rd->thr - gyro_y_pid.Output + gyro_x_pid.Output - gyro_z_pid.Output;
            motor3.speed = rd->thr + gyro_y_pid.Output - gyro_x_pid.Output - gyro_z_pid.Output;
            motor4.speed = rd->thr - gyro_y_pid.Output - gyro_x_pid.Output + gyro_z_pid.Output;
        }
        break;
    case FIX_HEIGHT:
            motor1.speed = rd->thr + gyro_y_pid.Output + gyro_x_pid.Output + gyro_z_pid.Output + fix_height_pid.Output;
            motor2.speed = rd->thr - gyro_y_pid.Output + gyro_x_pid.Output - gyro_z_pid.Output + fix_height_pid.Output;
            motor3.speed = rd->thr + gyro_y_pid.Output - gyro_x_pid.Output - gyro_z_pid.Output + fix_height_pid.Output;
            motor4.speed = rd->thr - gyro_y_pid.Output - gyro_x_pid.Output + gyro_z_pid.Output + fix_height_pid.Output;
        break;
    case FAIL:
        {
            motor1.speed = 0;
            motor2.speed = 0;
            motor3.speed = 0;
            motor4.speed = 0;
            xTaskNotifyGive(com_handle);
        }
        break;
    default:
        break;
    }

    motor1.speed = Motor_limit(motor1.speed, 0, 600);
    motor2.speed = Motor_limit(motor2.speed, 0, 600);
    motor3.speed = Motor_limit(motor3.speed, 0, 600);
    motor4.speed = Motor_limit(motor4.speed, 0, 600); 

    Motor_Set_Speed(&motor1);
    Motor_Set_Speed(&motor2);
    Motor_Set_Speed(&motor3);
    Motor_Set_Speed(&motor4);
}

void Flight_PID_Fix_Height()
{
    fix_height_pid.Target = fix_height;
    fix_height_pid.measure = My_LC08_Get_Distance();

    PID_Calc(&fix_height_pid);
}
