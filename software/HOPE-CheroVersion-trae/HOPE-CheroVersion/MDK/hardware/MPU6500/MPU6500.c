#include "MPU6500.h"
#include "SW_I2C_SEN.h"
#include "timer.h"
#include <math.h>
#include <stdio.h>

#define MPU6500_GYRO_SENS   16.4f     /* LSB / (deg/s) @ ±2000 dps */
#define MPU6500_ACCEL_SENS  2048.0f   /* LSB / g @ ±16g */

#define MPU6500_CALIB_NUM   200U      /* 陀螺仪零偏校准采样数 */

static float    s_fGyroBias[3] = {0.0f, 0.0f, 0.0f};
static float    s_fRoll  = 0.0f;
static float    s_fPitch = 0.0f;
static uint32_t s_u32LastTime = 0U;
static uint8_t  s_u8Ready = 0U;

static uint8_t MPU6500_WriteReg(uint8_t u8Reg, uint8_t u8Val)
{
    return SEN_SW_I2C_Write_SingleByte(MPU6500_I2C_ADDR, u8Reg, u8Val);
}

static uint8_t MPU6500_ReadReg(uint8_t u8Reg)
{
    return SEN_SW_I2C_Read_SingleByte(MPU6500_I2C_ADDR, u8Reg);
}

uint8_t MPU6500_ReadID(void)
{
    return MPU6500_ReadReg(MPU6500_REG_WHO_AM_I);
}

/* 读取 14 字节原始数据：加速度(6) + 温度(2) + 陀螺仪(6) */
static void MPU6500_ReadRaw(int16_t *ps16Acc, int16_t *ps16Gyro)
{
    uint8_t au8Buf[14];

    (void)SEN_SW_I2C_Read_MultiBytes(MPU6500_I2C_ADDR, MPU6500_REG_ACCEL_XOUT_H, 14U, au8Buf);

    ps16Acc[0]  = (int16_t)(((uint16_t)au8Buf[0]  << 8) | au8Buf[1]);
    ps16Acc[1]  = (int16_t)(((uint16_t)au8Buf[2]  << 8) | au8Buf[3]);
    ps16Acc[2]  = (int16_t)(((uint16_t)au8Buf[4]  << 8) | au8Buf[5]);
    ps16Gyro[0] = (int16_t)(((uint16_t)au8Buf[8]  << 8) | au8Buf[9]);
    ps16Gyro[1] = (int16_t)(((uint16_t)au8Buf[10] << 8) | au8Buf[11]);
    ps16Gyro[2] = (int16_t)(((uint16_t)au8Buf[12] << 8) | au8Buf[13]);
}

void MPU6500_Init(void)
{
    uint16_t u16i;
    int16_t  s16Acc[3], s16Gyro[3];
    float    fGxSum = 0.0f, fGySum = 0.0f, fGzSum = 0.0f;

    SEN_SW_I2C_Init();

    /* 复位 */
    (void)MPU6500_WriteReg(MPU6500_REG_PWR_MGMT_1, 0x80);
    DDL_DelayMS(100U);

    /* 唤醒：时钟源 = PLL（陀螺仪 X 轴参考） */
    (void)MPU6500_WriteReg(MPU6500_REG_PWR_MGMT_1, 0x01);
    /* 显式使能所有轴（加速度计 + 陀螺仪均不待机） */
    (void)MPU6500_WriteReg(MPU6500_REG_PWR_MGMT_2, 0x00);
    DDL_DelayMS(100U);   /* 陀螺仪启动稳定时间 */

    /* 采样率 1kHz；DLPF 关闭；陀螺仪 ±2000dps；加速度计 ±16g */
    (void)MPU6500_WriteReg(MPU6500_REG_SMPLRT_DIV, 0x00);
    (void)MPU6500_WriteReg(MPU6500_REG_CONFIG, 0x00);
    (void)MPU6500_WriteReg(MPU6500_REG_GYRO_CONFIG, 0x18);
    (void)MPU6500_WriteReg(MPU6500_REG_ACCEL_CONFIG, 0x18);
    (void)MPU6500_WriteReg(MPU6500_REG_ACCEL_CONFIG2, 0x03);
    DDL_DelayMS(10U);

    /* 陀螺仪零偏校准：期间请保持设备静止 */
    for (u16i = 0U; u16i < MPU6500_CALIB_NUM; u16i++)
    {
        MPU6500_ReadRaw(s16Acc, s16Gyro);
        fGxSum += (float)s16Gyro[0];
        fGySum += (float)s16Gyro[1];
        fGzSum += (float)s16Gyro[2];
        DDL_DelayUS(1000U);
    }
    s_fGyroBias[0] = fGxSum / (float)MPU6500_CALIB_NUM / MPU6500_GYRO_SENS;
    s_fGyroBias[1] = fGySum / (float)MPU6500_CALIB_NUM / MPU6500_GYRO_SENS;
    s_fGyroBias[2] = fGzSum / (float)MPU6500_CALIB_NUM / MPU6500_GYRO_SENS;

    /* 打印校准后的原始陀螺仪值，用于诊断陀螺仪是否输出 */
    printf("GYRO_RAW=%d,%d,%d\r\n", (int)s16Gyro[0], (int)s16Gyro[1], (int)s16Gyro[2]);

    s_fRoll       = 0.0f;
    s_fPitch      = 0.0f;
    s_u32LastTime = Time_ms;
    s_u8Ready     = 1U;
}

void MPU6500_GetAttitude(float *pfRoll, float *pfPitch)
{
    int16_t  s16Acc[3], s16Gyro[3];
    float    fAx, fAy, fAz, fGx, fGy, fGz;
    float    fRollAcc, fPitchAcc, fDt;
    uint32_t u32Now;

    if (0U == s_u8Ready)
    {
        *pfRoll  = 0.0f;
        *pfPitch = 0.0f;
        return;
    }

    u32Now = Time_ms;
    MPU6500_ReadRaw(s16Acc, s16Gyro);

    fAx = (float)s16Acc[0] / MPU6500_ACCEL_SENS;
    fAy = (float)s16Acc[1] / MPU6500_ACCEL_SENS;
    fAz = (float)s16Acc[2] / MPU6500_ACCEL_SENS;

    fGx = (float)s16Gyro[0] / MPU6500_GYRO_SENS - s_fGyroBias[0];
    fGy = (float)s16Gyro[1] / MPU6500_GYRO_SENS - s_fGyroBias[1];
    fGz = (float)s16Gyro[2] / MPU6500_GYRO_SENS - s_fGyroBias[2];

    fDt = (float)(u32Now - s_u32LastTime) / 1000.0f;
    s_u32LastTime = u32Now;
    if (fDt <= 0.0f)
    {
        fDt = 0.001f;
    }
    if (fDt > 0.1f)
    {
        fDt = 0.01f;
    }

    fRollAcc  = atan2f(fAy, fAz) * 57.29578f;
    fPitchAcc = atan2f(-fAx, sqrtf(fAy * fAy + fAz * fAz)) * 57.29578f;

    /* 互补滤波：98% 陀螺仪积分 + 2% 加速度计 */
    s_fRoll  = 0.98f * (s_fRoll  + fGx * fDt) + 0.02f * fRollAcc;
    s_fPitch = 0.98f * (s_fPitch + fGy * fDt) + 0.02f * fPitchAcc;

    *pfRoll  = s_fRoll;
    *pfPitch = s_fPitch;
}

void MPU6500_GetData(float *pfAcc, float *pfGyro)
{
    int16_t s16Acc[3], s16Gyro[3];

    if (0U == s_u8Ready)
    {
        pfAcc[0]  = 0.0f;
        pfAcc[1]  = 0.0f;
        pfAcc[2]  = 0.0f;
        pfGyro[0] = 0.0f;
        pfGyro[1] = 0.0f;
        pfGyro[2] = 0.0f;
        return;
    }

    MPU6500_ReadRaw(s16Acc, s16Gyro);

    pfAcc[0] = (float)s16Acc[0] / MPU6500_ACCEL_SENS;
    pfAcc[1] = (float)s16Acc[1] / MPU6500_ACCEL_SENS;
    pfAcc[2] = (float)s16Acc[2] / MPU6500_ACCEL_SENS;

    pfGyro[0] = (float)s16Gyro[0] / MPU6500_GYRO_SENS - s_fGyroBias[0];
    pfGyro[1] = (float)s16Gyro[1] / MPU6500_GYRO_SENS - s_fGyroBias[1];
    pfGyro[2] = (float)s16Gyro[2] / MPU6500_GYRO_SENS - s_fGyroBias[2];
}
