#include "FreeRTOS_Task.h"

// 通讯任务
void com_task(void *args);
TaskHandle_t com_handle;
#define com_Stack_Size          128
#define com_priority            3
#define com_period              6

// 按键任务
void key_task(void *args);
TaskHandle_t key_handle;
#define key_Stack_Size          128
#define key_priority            2
#define key_period              20

// 摇杆任务
void joy_task(void *args);
TaskHandle_t joy_handle;
#define joy_Stack_Size          128
#define joy_priority            2
#define joy_period              20

void FreeRTOS_Start(void)
{
    xTaskCreate(com_task, "com", com_Stack_Size, NULL, com_priority, &com_handle);
    xTaskCreate(key_task, "key", key_Stack_Size, NULL, key_priority, &key_handle);
    xTaskCreate(joy_task, "joy", joy_Stack_Size, NULL, joy_priority, &joy_handle);

    vTaskStartScheduler();
}

void com_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();

    TX_Mode();
    while(1)
    {
        Transmit_Data();
        vTaskDelayUntil(&Time_Start, com_period);
    }
}
void key_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();
    while (1)
    { 
        Key_Data();

        vTaskDelayUntil(&Time_Start, key_period);
    }
}

void joy_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();
    while (1)
    {
        Joystick_Data();

        vTaskDelayUntil(&Time_Start, joy_period);
    }
}
