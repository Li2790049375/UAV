#include "Data.h"

// 摇杆数据结构体
Joystick_Struct Joystick = {0};
Remote_Data remote_data = {0};

int16_t Key_pitch_offset = 0;
int16_t Key_roll_offset = 0;

// 记录摇杆偏移量
int16_t yaw_offset = 0;
int16_t thr_offset = 0;
int16_t pitch_offset = 0;
int16_t roll_offset = 0;

// 校准摇杆偏移
void calibration_joystick()
{
    int16_t yaw_sum= 0;
    int16_t thr_sum= 0;
    int16_t pitch_sum= 0;
    int16_t roll_sum= 0;

    Key_pitch_offset = 0;
    Key_roll_offset = 0;

    for(uint8_t i = 0; i < 10; i++)
    {
        Joystick_Data();
        yaw_sum += Joystick.yaw - 500;
        thr_sum += Joystick.thr - 0;
        pitch_sum += Joystick.pitch - 500;
        roll_sum += Joystick.roll - 500;
        vTaskDelay(10);
    }
    yaw_offset = yaw_sum / 10;
    thr_offset = thr_sum / 10;
    pitch_offset = pitch_sum / 10;  
    roll_offset = roll_sum / 10;
}

void Key_Data()
{
    // 上升沿检测
    static uint8_t last_key1 = 0, last_key2 = 0, last_key3 = 0, last_key4 = 0;
    static uint8_t last_key5 = 0, last_key6 = 0, last_key7 = 0, last_key8 = 0;

    Key_NUM Key = KeyNum();

    // Key1 急停
    if(Key == Key1 && last_key1 == 0) { remote_data.shutdown = 1; }
    last_key1 = (Key == Key1);

    // Key2 定高
    if(Key == Key2 && last_key2 == 0) { remote_data.fix_height = 1; }
    last_key2 = (Key == Key2);

    // Key3 校准摇杆
    if(Key == Key3 && last_key3 == 0) { calibration_joystick(); }
    last_key3 = (Key == Key3);

    // Key4 解锁
    if(Key == Key4 && last_key4 == 0) { remote_data.unlock = 1; }
    last_key4 = (Key == Key4);

    // Key5-8 微调
    if(Key == Key5 && last_key5 == 0)  { Key_roll_offset  -= 10; }
    last_key5 = (Key == Key5);
    if(Key == Key6 && last_key6 == 0)  { Key_roll_offset  += 10; }
    last_key6 = (Key == Key6);
    if(Key == Key7 && last_key7 == 0)  { Key_pitch_offset -= 10; }
    last_key7 = (Key == Key7);
    if(Key == Key8 && last_key8 == 0)  { Key_pitch_offset += 10; }
    last_key8 = (Key == Key8);

}

void Joystick_Data()
{
    Joystick_Struct local_joy;

    // ADC读取和数值换算不需要关中断
    Joystick_GetValue(&local_joy);

    local_joy.yaw   = local_joy.yaw   * 1000 / 4095 - yaw_offset;
    local_joy.thr   = local_joy.thr   * 1000 / 4095 - thr_offset;
    local_joy.pitch = local_joy.pitch * 1000 / 4095 - pitch_offset + Key_pitch_offset;
    local_joy.roll  = local_joy.roll  * 1000 / 4095 - roll_offset  + Key_roll_offset;

    // 限幅
    local_joy.yaw   = (local_joy.yaw   >= 1000) ? 1000 : ((local_joy.yaw   <= 0) ? 0 : local_joy.yaw);
    local_joy.thr   = (local_joy.thr   >= 1000) ? 1000 : ((local_joy.thr   <= 0) ? 0 : local_joy.thr);
    local_joy.pitch = (local_joy.pitch >= 1000) ? 1000 : ((local_joy.pitch <= 0) ? 0 : local_joy.pitch);
    local_joy.roll  = (local_joy.roll  >= 1000) ? 1000 : ((local_joy.roll  <= 0) ? 0 : local_joy.roll);

    // 临界区
    taskENTER_CRITICAL();
    remote_data.yaw   = local_joy.yaw;
    remote_data.thr   = local_joy.thr;
    remote_data.pitch = local_joy.pitch;
    remote_data.roll  = local_joy.roll;
    // 同步全局变量供calibration_joystick()使用
    Joystick = local_joy;   
    taskEXIT_CRITICAL();
}
