#include "user.h"

//https://github.com/CodingBugStd/Aerocraft

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t RxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t TxAddr[5] = {0x43,0x16,'F','L','Y'};		//飞机地址

//任务参数
uint16_t oled_fre = 40;		//oled刷新频率
uint16_t reply_delay = 100;	//最大等待主机信号时间
uint16_t mpu_fre = 100;		//mpu刷新频率

//任务句柄
TaskHandle_t RTOS_CreatTask_TaskHandle = NULL;
TaskHandle_t nRF24L01_Intterrupt_TaskHandle = NULL;
TaskHandle_t MPU_TaskHandle = NULL;
TaskHandle_t OLED_TaskHandle = NULL;
TaskHandle_t Reply_TaskHandle = NULL;

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果
SemaphoreHandle_t	Angle_occFlag = NULL;		//Angle变量的占用标志

//全局变量
Angular_Velocity_Struct	Angular = {0,0,0};
Angle_Struct Angle = {0,0,0};

void RTOS_CreatTask_Task(void*ptr);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	BSP_PWM_Init();
	BSP_Key_Init();
	BSP_Usart_Init();
	BSP_i2c_Init();

	OLED_Init();
	nRF24L01_Init();
	if(nRF24L01_Check()==1)
	{
		OLED_ShowString("nrf err",5,0,1);
		OLED_Refresh();
		while(nRF24L01_Check() == 1);
	}
	if(MPU9250_Check()==1)
	{
		OLED_ShowString("mpu err",6,0,1);
		OLED_Refresh();
	}

	//nRF24L01 相关配置
	nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
	nRF24_Cfg.retry = 5;	//最大重发次数
	nRF24_Cfg.retry_cycle = 1;	//重发周期
	nRF24_Cfg.Rx_Length = 32;	//结束长度
	MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
	MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
	nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L0
	nRF24L01_Rx_Mode();

	xTaskCreate(
		RTOS_CreatTask_Task,
		"CreatTask",
		144,
		NULL,
		15,
		&RTOS_CreatTask_TaskHandle
	);

	vTaskStartScheduler();
	while(1);
}

void RTOS_CreatTask_Task(void*ptr)
{
	nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);
	Angle_occFlag = xSemaphoreCreateMutex();

	#if 1
	xTaskCreate(
		Reply_Task,
		"reply",
		144,
		&reply_delay,
		11,
		&Reply_TaskHandle
	);
	#endif

	#if 1
	xTaskCreate(
		MPU_Task,
		"mpu",
		128,
		&mpu_fre,
		11,
		&MPU_TaskHandle
	);
	#endif

	#if 1
	xTaskCreate(
		nRF24L01_Intterrupt_Task,
		"NI task",
		72,
		NULL,
		13,
		&nRF24L01_Intterrupt_TaskHandle
	);
	#endif

	#if 1
	xTaskCreate(
		OLED_Task,
		"oled",
		128,
		&oled_fre,
		10,
		&OLED_TaskHandle
	);
	#endif

	vTaskDelete(NULL);

}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
	if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
}
