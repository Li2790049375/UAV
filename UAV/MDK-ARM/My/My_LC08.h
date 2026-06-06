#ifndef __MY_LC08_H__
#define __MY_LC08_H__

#include "main.h"
#include "i2c.h"
#include "Com_debug.h" 
#include "FreeRTOS.h"
#include "task.h"

/* LC08 激光光流模块 */
#define LC08_ADDR       0xC4    /* I2C地址(8位格式, 已左移) */
#define LC08_REG        0x33    /* 高度数据寄存器起始地址 */

void My_LC08_Init(void);
uint16_t My_LC08_Get_Distance(void);

#endif
