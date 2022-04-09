#ifndef _USER_H_
#define _USER_H_

#include "self_stm32f10x.h"
#include <math.h>

#include "BSP\bsp_usart.h"
#include "BSP\bsp_i2c.h"

#include "HARDWARE\nrf24\nrf24l01.h"
#include "HARDWARE\oled\oled.h"
#include "HARDWARE\mpu9250\mpu9250.h"

#include "SOFTWARE\vofa\vofa_p.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef enum
{
    Remote= 0,      //遥控
    Auto = 1,       //自动
    Adjust_PID = 2  //PID调参数
}RunMode_Type;

typedef struct
{
    uint8_t nrf_sigh;   //0:正常 其他:当前信号丢失的时长(单位:Reply任务的MaxWait)
    RunMode_Type mode;
}sysStatus;

void Reply_Task(void*ptr);
void MPU_Task(void*ptr);
void OLED_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);

#endif

