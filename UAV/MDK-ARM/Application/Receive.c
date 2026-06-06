#include "Receive.h"

extern Remote_Data remote_data;
extern Remote_State remote_state;
extern Flight_State flight_state;
extern uint16_t fix_height;

extern Euler_Struct euler_angle;
extern float pitch_trim;
extern float roll_trim;
extern float yaw_trim;

Thr_State thr_state = FREE;
uint32_t MAX_enter_Time = 0;
uint32_t MIN_enter_Time = 0;

uint8_t rx_buff[RX_PLOAD_WIDTH] = {0};
uint8_t retry_count = 0;

uint8_t Receive_Data(Remote_Data *out_data)
{
    memset(rx_buff, 0, RX_PLOAD_WIDTH);
    uint8_t nrf_res = NRF24L01_RxPacket(rx_buff);
    if(nrf_res != 0)
    {
        return 1;
    }

    if(rx_buff[0] != Frame_Head_Check_1 || rx_buff[1] != Frame_Head_Check_2)
    {
        return 1;
    }

    uint32_t sum = 0;
    uint32_t sum_receive;
    for(uint8_t i = 0; i < RX_PLOAD_WIDTH - 4; i++)
    {
        sum += rx_buff[i];
    }

    sum_receive = (uint32_t)rx_buff[RX_PLOAD_WIDTH - 4] << 24 | (uint32_t)rx_buff[RX_PLOAD_WIDTH - 3] << 16 | (uint32_t)rx_buff[RX_PLOAD_WIDTH - 2] << 8 | (uint32_t)rx_buff[RX_PLOAD_WIDTH - 1];
    
    if(sum != sum_receive)
    {
        return 1;
    }
    
    out_data->yaw = (rx_buff[2] << 8) | rx_buff[3];
    out_data->thr = (rx_buff[4] << 8) | rx_buff[5];
    out_data->pitch = (rx_buff[6] << 8) | rx_buff[7];
    out_data->roll = (rx_buff[8] << 8) | rx_buff[9];
    out_data->shutdown = rx_buff[10];
    out_data->fix_height = rx_buff[11];
    out_data->unlock = rx_buff[12];

    debug_printf(":%d,%d,%d,%d,%d,%d,%d\n", out_data->yaw, out_data->thr, out_data->pitch, out_data->roll, out_data->shutdown, out_data->fix_height, out_data->unlock);

    return 0;
}

void connect_state(uint8_t res)
{
    if(res == 0)
    {   
        if(remote_state == REMOTE_DISCONNECTED)
        {
            remote_state = REMOTE_CONNECTED_N;
        }
        retry_count = 0;
    }
    else if(res == 1)
    {
        retry_count ++;
        if(retry_count >= 50)
        {
            remote_state = REMOTE_DISCONNECTED;
            retry_count = 0;
        }
        
    }
}

// 解锁:0,上锁:1
static uint8_t check_unlock(void)
{
    switch (thr_state)
    {
    case FREE:
        if(remote_data.thr >= 900)
        {
            thr_state = MAX;
            MAX_enter_Time = xTaskGetTickCount();
        }
        break;
    case MAX:
        if(remote_data.thr < 900)
        {
            if(xTaskGetTickCount() - MAX_enter_Time >= 500)
            {
                thr_state = LEAVE_MAX;
            }
            else
            {
                thr_state = FREE;
            }
        }
        
        break;
    case LEAVE_MAX:
        if(remote_data.thr <= 100)
        {
            thr_state = MIN;
            MIN_enter_Time = xTaskGetTickCount();
        }
        break;
    case MIN:
        if(xTaskGetTickCount() - MIN_enter_Time < 500)
        {
            if(remote_data.thr > 100)
            {
                thr_state = FREE;
            }
        }
        else
        {
            thr_state = UNLOCK;
        }
        
        break;    
    case UNLOCK:
        /* code */
        break;
    default:
        break;
    }

    if(thr_state == UNLOCK)
    {
        return 0;
    }
    return 1;
}

void Flight_State_Update(void)
{
    switch (flight_state)
    {
    case IDLE:
        if(check_unlock() == 0)
        {
            pitch_trim = euler_angle.pitch;
            roll_trim  = euler_angle.roll;
            yaw_trim   = euler_angle.yaw;

            flight_state = NORMAL;
            thr_state = FREE;
            Flight_Reset_PID();
        }
        break;
    case NORMAL:
        if(remote_data.fix_height == 1)
        {
            flight_state = FIX_HEIGHT;
            remote_data.fix_height = 0;

            fix_height = My_LC08_Get_Distance();
        }
        if(remote_state == REMOTE_DISCONNECTED || remote_state == REMOTE_CONNECTED_N)
        {
            flight_state = FAIL;
        }
        break;
    case FIX_HEIGHT:
        if(remote_data.fix_height == 1)
        {
            flight_state = NORMAL;
            remote_data.fix_height = 0;
        }
        if(remote_state == REMOTE_DISCONNECTED || remote_state == REMOTE_CONNECTED_N)
        {
            flight_state = FAIL;
        }
        break;
    case FAIL:
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
        flight_state = IDLE;
        remote_state = REMOTE_CONNECTED_N;
        break;
    default:
        break;
    }
}
