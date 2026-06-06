#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "i2c.h"
#include "Config.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>

#define MPU6050_ADDR 0x68
#define MPU6050_ADDR_WRITE 0xD0
#define MPU6050_ADDR_READ 0xD1

#define	SMPLRT_DIV		0x19
#define	CONFIG			0x1A
#define	GYRO_CONFIG		0x1B
#define	ACCEL_CONFIG	0x1C
           
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
        
#define	PWR_MGMT_1		0x6B
#define	PWR_MGMT_2		0x6C
#define	WHO_AM_I		0x75

void MPU6050_Init(void);
void MPU6050_Get_Gyro(Gyro_Struct *gyro);
void MPU6050_Get_Accel(Accel_Struct *accel);
void MPU6050_Get_Data(Gyro_Accel_Struct *data);

#endif 
