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

void Reply_Task(void*ptr);
void MPU_Task(void*ptr);
void OLED_Task(void*ptr);
void nRF24L01_Intterrupt_Task(void*ptr);

#endif

