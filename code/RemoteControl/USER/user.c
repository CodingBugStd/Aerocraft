#include "user.h"

//任务参数
extern nRF24L01_Cfg nRF24_Cfg;

//任务句柄
extern TaskHandle_t nRF24L01_Intterrupt_TaskHandle ;    

//队列 信号
extern SemaphoreHandle_t	nRF24_ISRFlag;		//nrf24硬件中断标志
extern SemaphoreHandle_t	nRF24_RecieveFlag;	//nrf24接收标志(数据已经进入单片机,等待处理)
extern QueueHandle_t		nRF24_SendResult;	//nrf24发送结果

//全局变量
extern float Gyroscope[3];
extern uint8_t rockerInput[4];		//摇杆输入
extern nrfCount_Type nrf_count;

/*******************************************************************
 * 功能:freeRTOS下的nrf24通讯任务
 * 参数:(uint8_t*) 通讯频率 单位(hz)
 * 返回值:
 * 备注:
 * 2022/2/17   庞碧璋
 *******************************************************************/
void RemoteControl_Task(void*ptr)
{
    uint16_t delay_cycle = (1000 / *(uint8_t*)ptr) / portTICK_RATE_MS;    //通讯频率计算
    uint8_t*sbuffer = nRF24L01_Get_RxBufAddr();
    RemoteControl_Type  send;
    uint8_t sendResault = 0;
    TickType_t time = xTaskGetTickCount();  //获取当前系统时间
    while(1)
    {
        nRF24L01_Send((uint8_t*)&send,32);      //发送
        //等待nrf24中断(发送完成中断 或 未应答中断)
        while(xQueueReceive(nRF24_SendResult,&sendResault,delay_cycle/2) == pdFALSE)
        {
            //可能是nrf出现未知错误
            vTaskSuspend(nRF24L01_Intterrupt_TaskHandle);   //挂起中断任务
            nRF24L01_Write_Reg(0x07,0xE0);    //清除nrf24所有中断
            EXTI_ClearITPendingBit(NRF24L01_IQR_Line);          //挂起外部中断
            nRF24L01_Init();                   //重新初始化nrf
            nRF24L01_Config(&nRF24_Cfg);
            vTaskResume(nRF24L01_Intterrupt_TaskHandle);    //解挂
            nRF24L01_Send(sbuffer,32);
            time = xTaskGetTickCount();  //重新获取当前系统时间
        }
        nrf_count.SendCount++;
        if(sendResault)
        {
            nrf_count.SendAck_Count++;
            //接收到硬件ACK 说明从机的nrf24已经接收到
            //等待从机回复(这里等待不是nrf24硬件上的ACk信号,是从机上软件的回复)
            //等待时长 1/2 任务周期
            if(xSemaphoreTake(nRF24_RecieveFlag,delay_cycle/2) == pdFALSE)
            {
                //未接收到从机软件回复
                nrf_count.Slave_NoAckCount++;
            }else
            {
                //处理从机软件回复
                nrf_count.Slave_AckCoount++;
                MemCopy(sbuffer,(uint8_t*)Gyroscope,12);
            }
        }else
        {
            nrf_count.SendNoAck_Count++;
        }
        xTaskDelayUntil(&time,delay_cycle); 
    }
}

/*******************************************************************
 * 功能:摇杆输入
 * 参数:频率
 * 返回值:无
 * 2022/2/17   庞碧璋
 *******************************************************************/
void Rocker_Task(void*ptr)
{
    uint8_t cycle = (1000 / *(uint8_t*)ptr) / portTICK_PERIOD_MS;   //频率换算周期
    TickType_t  time = xTaskGetTickCount();
    while(1)
    {
        for(uint8_t temp=0;temp<4;temp++)
            rockerInput[temp] = Rocker_UnsignedGet(&rockers[temp]);
        vTaskDelayUntil(&time,cycle);
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

/*******************************************************************
 * 功能:通过串口定时打印运行状态
 * 参数:NULL
 * 返回值:无
 * 2022/2/17   庞碧璋
 *******************************************************************/
void User_FeedBack_Task(void*ptr)
{
    TickType_t time = xTaskGetTickCount();
    while(1)
    {
        #if 0
        //vTaskDelayUntil(&time,20/portTICK_PERIOD_MS);   //50Hz
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("********************************\r\n");
        printf("Send:%d\r\n",nrf_count.SendCount);
        printf("HD ACK:%d\r\n",nrf_count.SendAck_Count);
        printf("SF ACK:%d\r\n",nrf_count.Slave_AckCoount);
        #endif
        Vofa_Input(Gyroscope[0],0);
        Vofa_Input(Gyroscope[1],1);
        Vofa_Input(Gyroscope[2],2);
        Vofa_Input((float)nrf_count.SendAck_Count/100,3);
        Vofa_Input((float)nrf_count.Slave_AckCoount/100,4);
        Vofa_Send();
    }
}


