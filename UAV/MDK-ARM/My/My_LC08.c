/**
 * @file My_LC08.c
 * @brief LC08激光光流模块驱动实现
 * @details 用于读取LC08激光测距模块的高度数据
 */

#include "My_LC08.h"   // 包含LC08模块的头文件，定义了设备地址和寄存器

static uint16_t dist = 0;  // 静态变量：存储最近一次读取的高度值（单位：mm）

/**
 * @brief 初始化LC08激光模块
 * @details 向LC08模块发送读取命令，获取初始高度数据
 * @return 无
 */
void My_LC08_Init(void)
{
    uint8_t reg = LC08_REG;  // 要读取的寄存器地址（高度数据寄存器）
    uint8_t data[5];            // 存储读取到的5字节数据
    
    // 第一步：向LC08发送寄存器地址（写操作）
    // 参数：I2C句柄、设备地址、寄存器地址指针、数据长度、超时时间(ms)
    if(HAL_I2C_Master_Transmit(&hi2c1, LC08_ADDR, &reg, 1, 50) != HAL_OK) {
        debug_printf("LC08 Init: Transmit failed\n");  // 发送失败，打印错误信息
        return;  // 初始化失败，直接返回
    }
    
    // 第二步：从LC08读取5字节数据（读操作）
    // 参数：I2C句柄、设备地址、数据缓冲区、数据长度、超时时间(ms)
    if(HAL_I2C_Master_Receive(&hi2c1, LC08_ADDR, data, 5, 50) != HAL_OK) {
        debug_printf("LC08 Init: Receive failed\n");  // 接收失败，打印错误信息
        return;  // 初始化失败，直接返回
    }
    
    // 计算高度值：高字节data[0]左移8位，加上低字节data[1]
    dist = ((uint16_t)data[0] << 8) | data[1];
    
    // 打印初始化成功信息和初始高度值
    debug_printf("LC08 Init OK, height=%d\n", dist);
}

/**
 * @brief 获取LC08激光模块的距离（高度）值
 * @details 读取LC08模块的高度数据，返回最近一次有效测量值
 * @return 当前测量的高度值（单位：mm），读取失败时返回上一次有效值
 */
uint16_t My_LC08_Get_Distance(void)
{
    uint8_t reg = LC08_REG, data[5];  // 寄存器地址和数据缓冲区
    
    // 第一步：发送寄存器地址
    if(HAL_I2C_Master_Transmit(&hi2c1, LC08_ADDR, &reg, 1, 50) != HAL_OK) 
        return dist;  // 发送失败，返回上一次的高度值
    
    // 第二步：读取5字节数据
    if(HAL_I2C_Master_Receive(&hi2c1, LC08_ADDR, data, 5, 50) != HAL_OK) 
        return dist;  // 接收失败，返回上一次的高度值
    
    // 计算高度值
    uint16_t h = ((uint16_t)data[0] << 8) | data[1];
    
    // 如果读取到有效数据（高度>0），更新存储的高度值
    if(h > 0) dist = h;
    
    // 返回当前高度值
    return dist;
}
