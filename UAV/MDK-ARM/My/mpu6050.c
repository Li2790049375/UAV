#include "mpu6050.h"

int32_t accel_x_offset = 0;
int32_t accel_y_offset = 0;
int32_t accel_z_offset = 0;

int32_t gyro_x_offset = 0;
int32_t gyro_y_offset = 0;
int32_t gyro_z_offset = 0;

void MPU6050_Write_Reg(uint8_t reg, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR_WRITE, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

void MPU6050_Read_Reg(uint8_t reg, uint8_t *data)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR_READ, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 1000);
}

void MPU6050_calculate_offset()
{
    // 等待飞机平稳
    uint8_t Count = 0;
    Accel_Struct current_accel = {0};
    Accel_Struct last_accel = {0};

    while(Count < 100)
    {
        MPU6050_Get_Accel(&current_accel);
        if(abs(current_accel.accel_x - last_accel.accel_x) < 400 && abs(current_accel.accel_y - last_accel.accel_y) < 400 && abs(current_accel.accel_z - last_accel.accel_z) < 400)
        {
            Count++;
        }
        else
        {
            Count = 0;
        }
        last_accel = current_accel;
        vTaskDelay(6);
    }

    // 校准陀螺仪
    Gyro_Accel_Struct gyro_accel_data = {0};
    int32_t accel_x_offset_sum = 0;
    int32_t accel_y_offset_sum = 0;
    int32_t accel_z_offset_sum = 0;
    int32_t gyro_x_offset_sum = 0;
    int32_t gyro_y_offset_sum = 0;
    int32_t gyro_z_offset_sum = 0;
    for(uint8_t i = 0; i < 100; i++)
    {
        MPU6050_Get_Data(&gyro_accel_data);
        accel_x_offset_sum += (gyro_accel_data.accel.accel_x) - 0;
        accel_y_offset_sum += (gyro_accel_data.accel.accel_y) - 0;
        accel_z_offset_sum += (gyro_accel_data.accel.accel_z) - (32768 / 2);      // 1g为16384

        gyro_x_offset_sum += (gyro_accel_data.gyro.gyro_x) - 0;
        gyro_y_offset_sum += (gyro_accel_data.gyro.gyro_y) - 0;
        gyro_z_offset_sum += (gyro_accel_data.gyro.gyro_z) - 0;
        
        vTaskDelay(6);
    }

    accel_x_offset = accel_x_offset_sum / 100;
    accel_y_offset = accel_y_offset_sum / 100;
    accel_z_offset = accel_z_offset_sum / 100;

    gyro_x_offset = gyro_x_offset_sum / 100;
    gyro_y_offset = gyro_y_offset_sum / 100;
    gyro_z_offset = gyro_z_offset_sum / 100;
}

void MPU6050_Init(void)
{
    // 重启芯片，电源管理寄存器地址0x6B
    MPU6050_Write_Reg(0x6B, 0x80);
    uint8_t data = 0;
    uint16_t timeout = 0;
    while(data != 0x40)
    {
        MPU6050_Read_Reg(0x6B, &data);
        if(++timeout > 1000) return;
    }
    MPU6050_Write_Reg(0x6B, 0x00);

    // 设置角速度量程±2000°/s 
    MPU6050_Write_Reg(0x1B, 0x18);
    // 设置加速度量程±2g
    MPU6050_Write_Reg(0x1C, 0x00);
    // 关闭中断使能(未用到)
    MPU6050_Write_Reg(0x38, 0x00);
    // 用户配置寄存器(未使用FIFO队列，未使用扩展I2C)
    MPU6050_Write_Reg(0x6A, 0x00);
    // 设置采样频率，默认为1000HZ(1ms)
    MPU6050_Write_Reg(0x19, 0x01);
    // 设置加速度低通滤波184HZ，角速度低通滤波188HZ
    MPU6050_Write_Reg(0x1A, 1);
    // 为使用的系统时钟添加PLL
    MPU6050_Write_Reg(0x6B, 0x01);
    // 使能加速度计传感器和角速度传感器
    MPU6050_Write_Reg(0x6C, 0x00);

    // 陀螺仪零偏校准
    MPU6050_calculate_offset();
}

void MPU6050_Get_Gyro(Gyro_Struct *gyro)
{
    uint8_t hight = 0;
    uint8_t low = 0;
    MPU6050_Read_Reg(GYRO_XOUT_H, &hight);
    MPU6050_Read_Reg(GYRO_XOUT_L, &low);
    gyro->gyro_x = ((hight << 8 | low) - gyro_x_offset);

    MPU6050_Read_Reg(GYRO_YOUT_H, &hight);
    MPU6050_Read_Reg(GYRO_YOUT_L, &low);
    gyro->gyro_y = ((hight << 8 | low) - gyro_y_offset);

    MPU6050_Read_Reg(GYRO_ZOUT_H, &hight);
    MPU6050_Read_Reg(GYRO_ZOUT_L, &low);
    gyro->gyro_z = ((hight << 8 | low) - gyro_z_offset);
}

void MPU6050_Get_Accel(Accel_Struct *accel)
{
    uint8_t hight = 0;
    uint8_t low = 0;
    MPU6050_Read_Reg(ACCEL_XOUT_H, &hight);
    MPU6050_Read_Reg(ACCEL_XOUT_L, &low);
    accel->accel_x = ((hight << 8 | low) - accel_x_offset);

    MPU6050_Read_Reg(ACCEL_YOUT_H, &hight);
    MPU6050_Read_Reg(ACCEL_YOUT_L, &low);
    accel->accel_y = ((hight << 8 | low) - accel_y_offset);

    MPU6050_Read_Reg(ACCEL_ZOUT_H, &hight);
    MPU6050_Read_Reg(ACCEL_ZOUT_L, &low);
    accel->accel_z = ((hight << 8 | low) - accel_z_offset);
}

void MPU6050_Get_Data(Gyro_Accel_Struct *data)
{
    MPU6050_Get_Gyro(&data->gyro);
    MPU6050_Get_Accel(&data->accel);
}
