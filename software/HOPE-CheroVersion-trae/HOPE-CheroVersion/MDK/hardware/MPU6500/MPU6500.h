#ifndef __MPU6500_H
#define __MPU6500_H

#include "hc32_ll.h"

#define MPU6500_I2C_ADDR            0x68U

#define MPU6500_REG_SMPLRT_DIV      0x19U
#define MPU6500_REG_CONFIG          0x1AU
#define MPU6500_REG_GYRO_CONFIG     0x1BU
#define MPU6500_REG_ACCEL_CONFIG    0x1CU
#define MPU6500_REG_ACCEL_CONFIG2   0x1DU
#define MPU6500_REG_ACCEL_XOUT_H    0x3BU
#define MPU6500_REG_TEMP_OUT_H      0x41U
#define MPU6500_REG_GYRO_XOUT_H     0x43U
#define MPU6500_REG_PWR_MGMT_1      0x6BU
#define MPU6500_REG_PWR_MGMT_2      0x6CU
#define MPU6500_REG_WHO_AM_I        0x75U

#define MPU6500_WHO_AM_I_VAL        0x70U

uint8_t MPU6500_ReadID(void);
void    MPU6500_Init(void);
void    MPU6500_GetAttitude(float *pfRoll, float *pfPitch);
void    MPU6500_GetData(float *pfAcc, float *pfGyro);

#endif
