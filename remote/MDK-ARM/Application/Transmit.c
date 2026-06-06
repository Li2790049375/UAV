#include "Transmit.h"

extern Remote_Data remote_data;
uint8_t com_buf[TX_PLOAD_WIDTH] = {0};

#define COMMAND_REPEAT_COUNT 3

void Transmit_Data()
{
    uint32_t check_sum = 0;
    Remote_Data local_data;
    uint8_t tx_res;
    static uint8_t shutdown_repeat = 0;
    static uint8_t fix_height_repeat = 0;
    static uint8_t unlock_repeat = 0;
    
    // 临界区
    taskENTER_CRITICAL();
    if(remote_data.shutdown)
    {
        shutdown_repeat = COMMAND_REPEAT_COUNT;
        remote_data.shutdown = 0;
    }
    if(remote_data.fix_height)
    {
        fix_height_repeat = COMMAND_REPEAT_COUNT;
        remote_data.fix_height = 0;
    }
    if(remote_data.unlock)
    {
        unlock_repeat = COMMAND_REPEAT_COUNT;
        remote_data.unlock = 0;
    }

    local_data = remote_data;
    local_data.shutdown   = (shutdown_repeat > 0) ? 1 : 0;
    local_data.fix_height = (fix_height_repeat > 0) ? 1 : 0;
    local_data.unlock     = (unlock_repeat > 0) ? 1 : 0;
    taskEXIT_CRITICAL();

    // 帧头
    com_buf[0] = Frame_Head_Check_1;
    com_buf[1] = Frame_Head_Check_2;

    // 数据
    com_buf[2] = (local_data.yaw >> 8) & 0xFF;
    com_buf[3] = local_data.yaw & 0xFF;

    com_buf[4] = (local_data.thr >> 8) & 0xFF;
    com_buf[5] = local_data.thr & 0xFF;

    com_buf[6] = (local_data.pitch >> 8) & 0xFF;
    com_buf[7] = local_data.pitch & 0xFF;

    com_buf[8] = (local_data.roll >> 8) & 0xFF;
    com_buf[9] = local_data.roll & 0xFF;

    com_buf[10] = local_data.shutdown;
    com_buf[11] = local_data.fix_height;
    com_buf[12] = local_data.unlock;
    
    // 校验和
    for(uint8_t i = 0; i < TX_PLOAD_WIDTH  - 4; i++)
    {
        check_sum += com_buf[i];
    }

    com_buf[13] = (check_sum >> 24) & 0xFF;
    com_buf[14] = (check_sum >> 16) & 0xFF;
    com_buf[15] = (check_sum >> 8) & 0xFF;
    com_buf[16] = check_sum & 0xFF;

    // debug_printf(":%d,%d,%d,%d,%d,%d,%d\n", local_data.yaw, local_data.thr, local_data.pitch, local_data.roll, local_data.shutdown, local_data.fix_height, local_data.unlock);

    tx_res = NRF24L01_TxPacket(com_buf);
    if(tx_res == TX_OK)
    {
        if(shutdown_repeat > 0)   shutdown_repeat--;
        if(fix_height_repeat > 0) fix_height_repeat--;
        if(unlock_repeat > 0)     unlock_repeat--;
    }
}
