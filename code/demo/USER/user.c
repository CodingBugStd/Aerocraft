#include "user.h"

//任务句柄
extern TaskHandle_t RTOS_CreatTask_TaskHandle;
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle;

//队列句柄
extern SemaphoreHandle_t	nRF24_ISRFlag;		//nrf24硬件中断标志
extern SemaphoreHandle_t	nRF24_RecieveFlag;	//nrf24接收标志(数据已经进入单片机,等待处理)
extern QueueHandle_t		nRF24_SendResult;	//nrf24发送结果

//全局变量
extern Angular_Velocity_Struct Angular;
extern Angle_Struct Angle;

//主机回复任务
void Reply_Task(void*ptr)
{
    uint8_t MaxWait = *(uint8_t*)ptr / portTICK_RATE_MS;    //换算成心跳周期
    uint8_t*sbuf = nRF24L01_Get_RxBufAddr();    //nrf缓存地址
    uint8_t resualt;        //发射结果接收
    while(1)    
    {
        while(xSemaphoreTake(nRF24_RecieveFlag,MaxWait) == pdFALSE)
        {
            //过长时间没有接收到主机信号
            //紧急停止代码
            //...
        }
        //拷贝接收到的数据
        //回复主机
        MemCopy((uint8_t*)&Angle,sbuf,12);
        nRF24L01_Send(sbuf,32);
        xQueueReceive(nRF24_SendResult,&resualt,MaxWait);   //等待回复结果 -> nRF24L01_Send()
    }
}

/*******************************************************************
 * 功能:freeRTOS下的nrf24中断处理函数
 * 参数:NULL
 * 返回值:无
 * 备注:在isr外处理nrf24的中断,需要isr发出nRF24_ISRFlag信号
 * 2022/2/17   庞碧璋
 *******************************************************************/
void nRF24L01_Intterrupt_Task(void*ptr)
{
    while(1)
    {
        xSemaphoreTake(nRF24_ISRFlag,portMAX_DELAY);    //无限期等待
        nRF24L01_InterruptHandle();     //isr处理函数
    }
}

void MPU_Task(void*ptr)
{
    uint16_t cycle = 1000 / *(uint16_t*)ptr / portTICK_PERIOD_MS;
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        MPU9250_Get_Angular_velocity(&Angular);
        MPU9250_Get_Angle(&Angle);
        xTaskDelayUntil(&time,cycle);
    }
}

void OLED_Task(void*ptr)
{
    uint16_t cycle = (1000 / *(uint16_t*)ptr) / portTICK_PERIOD_MS;
    TickType_t  time = xTaskGetTickCount();
    uint8_t sbuf[32];
    while(1)
    {
            sprintf((char*)sbuf,"%3.2f",Angle.pitch);
            OLED_ShowString(sbuf,0,0,1);

            sprintf((char*)sbuf,"%3.2f",Angle.roll);
            OLED_ShowString(sbuf,1,0,1);

            sprintf((char*)sbuf,"%3.2f",Angle.yaw);
            OLED_ShowString(sbuf,2,0,1);

            OLED_ShowNum(xTaskGetTickCount()*portTICK_PERIOD_MS/1000,7,0,1);
            OLED_Refresh();
        vTaskDelayUntil(&time,cycle);
    }
}
