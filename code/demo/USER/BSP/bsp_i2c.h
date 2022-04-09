#ifndef _BSP_I2C_H_
#define _BSP_I2C_H_

#include "self_stm32f10x.h"

/***********************************************************************************
 * 软件模拟I2C
 * 依赖self_stm32f10x.h
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * date: 2022/3/30
**********************************************************************************/

#define OLED_I2C_SDA_GPIO   GPIOC
#define OLED_I2C_SDA_Pin    GPIO_Pin_11
#define OLED_I2C_SCL_GPIO   GPIOC
#define OLED_I2C_SCL_Pin    GPIO_Pin_12

#define EEROM_I2C_SDA_GPIO  GPIOB
#define EEROM_I2C_SDA_Pin   GPIO_Pin_0
#define EEROM_I2C_SCL_GPIO  GPIOB
#define EEROM_I2C_SCL_Pin   GPIO_Pin_1

#define MPU9250_I2C_SDA_GPIO    GPIOA
#define MPU9250_I2C_SDA_Pin     GPIO_Pin_5
#define MPU9250_I2C_SCL_GPIO    GPIOA
#define MPU9250_I2C_SCL_Pin     GPIO_Pin_7

typedef enum
{
    OLED_IIC = 0,
    EEPROM_IIC = 1,
    MPU9250_IIC = 2
}i2c_device;

typedef struct
{
    uint8_t delay;  //间接控制i2c频率,约低频率越高
    Pin SDA;
    Pin SCL;
}i2c_busType;


void BSP_i2c_Init(void);
void I2C_Start(i2c_device device);
void I2C_Stop(i2c_device device);
void I2C_SendByte(i2c_device device,uint8_t byte);
uint8_t I2C_ReadByte(i2c_device device);
uint8_t I2C_WaitAck(i2c_device device);
uint8_t I2C_NoWaitACK(i2c_device device);
void I2C_Ack(i2c_device device);
void I2C_Nack(i2c_device device);

#endif

