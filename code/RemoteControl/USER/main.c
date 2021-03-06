#include "self_stm32f10x.h"

#include "user.h"

#pragma	diag_suppress	870	//屏蔽汉字警告

//nRF24L01初始化结构体
nRF24L01_Cfg nRF24_Cfg;
static uint8_t TxAddr[5] = {0x43,0x16,'R','C',0xFF};	//遥控器地址
static uint8_t RxAddr[5] = {0x43,0x16,'F','L','Y'};		//飞机地址

//任务参数
uint8_t SendFre = 50;	//nrf24通讯频率
uint8_t RockerFre = 50;	//摇杆采样频率

//任务句柄
TaskHandle_t RTOS_CreatTask_TaskHandle = NULL;
TaskHandle_t RemoteControl_TaskHandle = NULL;
TaskHandle_t nRF24L01_Intterrupt_TaskHandle = NULL;
TaskHandle_t User_FeedBack_TaskHandle = NULL;
TaskHandle_t Rocker_TaskHandle = NULL;

//队列句柄
SemaphoreHandle_t	nRF24_ISRFlag = NULL;		//nrf24硬件中断标志
SemaphoreHandle_t	nRF24_RecieveFlag = NULL;	//nrf24接收标志(数据已经进入单片机,等待处理)
QueueHandle_t		nRF24_SendResult = NULL;	//nrf24发送结果

//全局变量
float Gyroscope[3];		//船只返回的姿态 boatGyroscope_occFlag保护
uint8_t rockerInput[4];		//摇杆输入
nrfCount_Type nrf_count = {0,0,0,0,0};
void RTOS_CreatTask_Task(void*ptr);

int main(void)
{
	uint8_t nrf_err;

	//中断优先级分组 4位主优先级 0位子优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//关闭JTAG调试接口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	//串口初始化
	BSP_Usart_Init();
	BSP_ADC_Init();	//ADC初始化

	//nRF24L01初始化
	nrf_err = nRF24L01_Init();
	if(nrf_err)
	{
		printf("nrf err\r\n");
	}else
	{
		//nRF24L01 相关配置
		nRF24_Cfg.Channel = 50;	//2.45GHz 通讯频段
		nRF24_Cfg.retry = 5;	//最大重发次数
		nRF24_Cfg.retry_cycle = 1;	//重发周期
		nRF24_Cfg.Rx_Length = 32;	//结束长度
		MemCopy(TxAddr,nRF24_Cfg.TX_Addr,5);
		MemCopy(RxAddr,nRF24_Cfg.RX_Addr,5);
		nRF24L01_Config(&nRF24_Cfg);	//配置nRF24L01	
		printf("nrf pass\r\n");
		nRF24L01_Rx_Mode();
	}

#if 1
	xTaskCreate(
		RTOS_CreatTask_Task,
		"CreatTask",
		144,
		NULL,
		15,
		&RTOS_CreatTask_TaskHandle
	);

	vTaskStartScheduler();
#endif

	while(1)
	{

	}
}

void RTOS_CreatTask_Task(void*ptr)
{
    nRF24_ISRFlag = xSemaphoreCreateBinary();
	nRF24_RecieveFlag = xSemaphoreCreateBinary();
	nRF24_SendResult = xQueueCreate(1,1);

    xTaskCreate(
		RemoteControl_Task,
		"RC task",
		512,
		(void*)&SendFre,
		12,
		&RemoteControl_TaskHandle
	);
	xTaskCreate(
		Rocker_Task,
		"RK",
		32,
		(void*)&RockerFre,
		9,
		&Rocker_TaskHandle
	);
	xTaskCreate(
		nRF24L01_Intterrupt_Task,
		"NI task",
		72,
		NULL,
		13,
		&nRF24L01_Intterrupt_TaskHandle
	);
	xTaskCreate(
		User_FeedBack_Task,
		"UFB task",
		64,
		NULL,
		11,
		&User_FeedBack_TaskHandle
	);
    vTaskDelete(NULL);
}
