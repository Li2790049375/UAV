#include "FreeRTOS_Task.h"

// LED结构体
LED_Struct led1 = {.tim = &htim3, .channel = TIM_CHANNEL_1, .pwm = 1000};
LED_Struct led2 = {.tim = &htim3, .channel = TIM_CHANNEL_2, .pwm = 1000};
LED_Struct led3 = {.tim = &htim3, .channel = TIM_CHANNEL_3, .pwm = 1000};
LED_Struct led4 = {.tim = &htim3, .channel = TIM_CHANNEL_4, .pwm = 1000};

// 遥控连接状态
Remote_State remote_state = REMOTE_DISCONNECTED;
Remote_Data remote_data = {.thr = 0, .yaw = 500, .pitch = 500, .roll = 500, .shutdown = 0, .fix_height = 0, .unlock = 0};
Flight_State flight_state = IDLE;
uint16_t fix_height = 0;

// 飞行任务
void flight_task(void *args);
TaskHandle_t flight_handle;
#define flight_Stack_Size       400
#define flight_priority         3
#define flight_period           6

// 灯光任务
void LED_task(void *args);
TaskHandle_t LED_handle;
#define LED_Stack_Size          128
#define LED_priority            1
#define LED_period              100

// 通讯任务
void com_task(void *args);
TaskHandle_t com_handle;
#define com_Stack_Size          128
#define com_priority            2
#define com_period              6


void FreeRTOS_Start(void)
{
    xTaskCreate(flight_task, "flight", flight_Stack_Size, NULL, flight_priority, &flight_handle);
    xTaskCreate(LED_task, "LED", LED_Stack_Size, NULL, LED_priority, &LED_handle);
    xTaskCreate(com_task, "com", com_Stack_Size, NULL, com_priority, &com_handle);

    vTaskStartScheduler();
}

void flight_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();
    static uint8_t count = 0;

    Flight_Init();
    Flight_Reset_PID();
    
    while (1)
    {
        Remote_Data local_rd;
        taskENTER_CRITICAL();
        local_rd = remote_data;
        taskEXIT_CRITICAL();

        if(flight_state != IDLE)
        {
            Flight_Get_Euler_Angle();

            Flight_PID(&local_rd);

            if(flight_state == FIX_HEIGHT)
            {
                count++;
                if(count >= 4)
                {
                    Flight_PID_Fix_Height();
                    count = 0;
                }
            }

            Flight_Motor(&local_rd);

            // uint16_t distance = My_LC08_Get_Distance();
            // debug_printf("dist=%d\n", distance);
        }
        else
        {
            Flight_Get_Euler_Angle();
            Flight_Motor(&local_rd);
        }

        vTaskDelayUntil(&Time_Start, flight_period);
    }
}

void LED_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();

    LED_Start(&led1);
    LED_Start(&led2);
    LED_Start(&led3);
    LED_Start(&led4);

    uint8_t Count = 0;
    while (1)
    {
        Count++;

        if(remote_state == REMOTE_DISCONNECTED)
        {   
            if(Count % 15 == 0)
            {
                led1.pwm = (led1.pwm == 1000) ? 500 : 1000;
                led2.pwm = (led2.pwm == 1000) ? 500 : 1000;
                led3.pwm = (led3.pwm == 1000) ? 500 : 1000;
                led4.pwm = (led4.pwm == 1000) ? 500 : 1000;
            }
        }
        else if(remote_state == REMOTE_CONNECTED_N)
        {
            if(Count % 5 == 0)
            {
                led1.pwm = (led1.pwm == 1000) ? 500 : 1000;
                led2.pwm = (led2.pwm == 1000) ? 500 : 1000;
                led3.pwm = (led3.pwm == 1000) ? 500 : 1000;
                led4.pwm = (led4.pwm == 1000) ? 500 : 1000;
            }
        }
        else if(remote_state == REMOTE_CONNECTED_Y)
        {
            if(flight_state == IDLE)
            {
                if(Count % 2 == 0)
                {
                    led1.pwm = (led1.pwm == 1000) ? 500 : 1000;
                    led2.pwm = (led2.pwm == 1000) ? 500 : 1000;
                    led3.pwm = (led3.pwm == 1000) ? 500 : 1000;
                    led4.pwm = (led4.pwm == 1000) ? 500 : 1000;
                }
            }
            else if(flight_state == NORMAL)
            {
                led1.pwm = 500;
                led2.pwm = 500;
                led3.pwm = 500;
                led4.pwm = 500;
            }
            else if(flight_state == FIX_HEIGHT)
            {
                led1.pwm = 1000;
                led2.pwm = 1000;
                led3.pwm = 1000;
                led4.pwm = 1000;
            }
            else if(flight_state == FAIL)
            {
                led1.pwm = 1000;
                led2.pwm = 1000;
                led3.pwm = 500;
                led4.pwm = 500;
            }
        }

        if(Count >= 30)
        {
            Count = 0;
        }
        
        LED_Set_PWM(&led1);
        LED_Set_PWM(&led2);
        LED_Set_PWM(&led3);
        LED_Set_PWM(&led4);

        vTaskDelayUntil(&Time_Start, LED_period);   
    }
}

uint8_t com_buf[TX_PLOAD_WIDTH] = {0};

void com_task(void *args)
{
    TickType_t Time_Start = xTaskGetTickCount();
    while (1)
    {
        Remote_Data rx_data = {0};
        uint8_t res = Receive_Data(&rx_data);
        connect_state(res);

        if(res == 0)
        {
            taskENTER_CRITICAL();
            remote_data = rx_data;
            taskEXIT_CRITICAL();

            if(rx_data.shutdown == 1)
            {
                remote_state = REMOTE_CONNECTED_N;
            }

            if(rx_data.unlock == 1 && remote_state == REMOTE_CONNECTED_N)
            {
                remote_state = REMOTE_CONNECTED_Y;
            }
        }

        Flight_State_Update();

        vTaskDelayUntil(&Time_Start, com_period);
    }
}
