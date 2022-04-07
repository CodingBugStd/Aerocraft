#include "user.h"

extern SemaphoreHandle_t nRF24_ISRFlag;
extern SemaphoreHandle_t nRF24_RecieveFlag;
extern QueueHandle_t    nRF24_SendResult;


void EXTI4_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;   //用于判断这个发出的信号量是否会引起其他高优先级的任务解除阻塞
    if(EXTI_GetITStatus(NRF24L01_IQR_Line) == SET)
    {
        EXTI_ClearITPendingBit(NRF24L01_IQR_Line);          //挂起中断
        if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)   //判断系统是否已经启动
        {
            xSemaphoreGiveFromISR(nRF24_ISRFlag,&xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);       //判断是否需要进行上下文切换(任务调度)
        }    
    }
}

//下面三个isr并不是真的isr,因为由RTOS的中断处理任务调用!
//并不是在硬件的中断服务函数中运行,使用不需要使用RTOS的
//FromISR API函数
static uint8_t send_result;

void nRF24L01_NoACK_ISR(void)
{
    send_result = 0;
    xQueueSend(nRF24_SendResult,&send_result,0);
}

void nRF24L01_Tx_ISR(void)
{
    send_result = 1;
    xQueueSend(nRF24_SendResult,&send_result,0);
}

void nRF24L01_Rx_ISR(void)
{
    xSemaphoreGive(nRF24_RecieveFlag);
}


