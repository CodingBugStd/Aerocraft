#include "bsp_usart.h"

USER_USART_Type usart1;
USER_USART_Type usart2;
USER_USART_Type usart3;

USER_USART_Type* usart_list[3] = {&usart1,&usart2,&usart3};

static uint8_t usart1_Rx_Sbuffer[64];
static uint8_t usart1_Tx_Sbuffer[64];
static uint8_t usart2_Rx_Sbuffer[64];
static uint8_t usart2_Tx_Sbuffer[64];
static uint8_t usart3_Rx_Sbuffer[64];

static void usart_def(void);

void usart_def(void)
{
    usart1.Usart = USART1;
    usart1.DMATx_Ch = DMA1_Channel4;
    usart1.RxSbuffer_MaxLen = 64;
    usart1.TxSbuffer_MaxLen = 64;
    usart1.RxSbuffer_Len = 0;
    usart1.TxSbuffer_Len = 0;
    usart1.RxSbuffer = usart1_Rx_Sbuffer;
    usart1.TxSbuffer = usart1_Tx_Sbuffer;

    usart2.Usart = USART2;
    usart2.DMATx_Ch = DMA1_Channel7;
    usart2.RxSbuffer_MaxLen = 64;
    usart2.TxSbuffer_MaxLen = 64;
    usart2.RxSbuffer_Len = 0;
    usart2.TxSbuffer_Len = 0;
    usart2.RxSbuffer = usart2_Rx_Sbuffer;
    usart2.TxSbuffer = usart2_Tx_Sbuffer;

    usart3.Usart = USART3;
    usart3.DMATx_Ch = NULL;
    usart3.RxSbuffer_MaxLen = 64;
    usart3.TxSbuffer_MaxLen = 0;
    usart3.RxSbuffer_Len = 0;
    usart3.TxSbuffer_Len = 0;
    usart3.RxSbuffer = usart3_Rx_Sbuffer;
    usart3.TxSbuffer = NULL;
}

void BSP_Usart_Init(void)
{
    usart_def();
    USART_GPIO_Init();
    USART_Config();
    USART_NVIC_Config();
    USART_DMA_Config();
    for(uint8_t temp=0;temp<3;temp++)
        USART_Cmd(usart_list[temp]->Usart,ENABLE);
}

void USART_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2,ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3,ENABLE);

    //串口1
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    //串口2
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD,&GPIO_InitStruct);

    //串口3
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOD,&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOD,&GPIO_InitStruct);
}

void USART_Config(void)
{
    USART_InitTypeDef   USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;

    for(uint8_t temp=0;temp<3;temp++)
        USART_Init(usart_list[temp]->Usart,&USART_InitStruct);

}

void USART_NVIC_Config(void)
{
    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);

    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    USART_ClearITPendingBit(USART3,USART_IT_RXNE);
}

void USART_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;

    //配置DMA发送
    for(uint8_t temp=0;temp<3;temp++)
    {
        if(usart_list[temp]->DMATx_Ch != NULL)
        {
            DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&usart_list[temp]->Usart->DR;
            DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)usart_list[temp]->TxSbuffer;

            DMA_Init(usart_list[temp]->DMATx_Ch,&DMA_InitStruct);
            USART_DMACmd(usart_list[temp]->Usart,USART_DMAReq_Tx,ENABLE);
        }
    }
    
    //DMA_ClearFlag(DMA1_FLAG_TC1);

    //配置DMA发送中断向量表
    NVIC_InitTypeDef    NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 8;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    //使能DMA接收中断
    for(uint8_t temp=0;temp<3;temp++)
    {
        if(usart_list[temp]->DMATx_Ch != NULL)
        {
            DMA_ITConfig(usart_list[temp]->DMATx_Ch,DMA_IT_TC,ENABLE);
        }
    }
}

/**************************************************************************
 * 功能:清除串口接收缓存区前len字节
 * 参数:
 * usart_handle:串口句柄地址 -> 见bsp_usart.h
 * len:要清除的长度
 * 返回值:无
 * 2022/3/11   庞碧璋
 ***********************************************************************/
void USART_RxSbuffer_Push(USER_USART_Type*usart_handle,uint8_t len)
{
    if(usart_handle->RxSbuffer_Len < len)
    {
        uint8_t temp;
        usart_handle->RxSbuffer_Len -= len;
        for(temp=0; temp < usart_handle->RxSbuffer_Len ; temp++)
            usart_handle->RxSbuffer[temp] = usart_handle->RxSbuffer[temp+len];
        return;
    }
    usart_handle->RxSbuffer_Len = 0;
}

/**************************************************************************
 * 功能:使用串口发送(堵塞式)
 * 参数:
 * usart_handle:串口句柄地址 -> 见bsp_usart.h
 * dat:要发送的数据的地址
 * len:要清除的长度
 * 返回值:
 * 0:发送成功
 * 1:串口被DMA占用或其他原因,发送失败
 * 2022/3/11   庞碧璋
 ***********************************************************************/
uint8_t Usart_Send(USER_USART_Type*usart_handle,uint8_t *dat,uint8_t len)
{   
    //判断这个通道有没有使用发送DMA
    if(usart_handle->DMATx_Ch != NULL)
    {
        //判断DMA是否正在占用串口
        if(usart_handle->TxSbuffer_Len == 1)
            return 1;
        for(uint8_t temp=0;temp<len;temp++)
        {
            USART_SendData(usart_handle->Usart,*dat);
	        while(USART_GetFlagStatus(usart_handle->Usart,USART_FLAG_TXE) == RESET);
        }
        return 0;
    }
    //没有使能DMA的串口发送
    for(uint8_t temp=0;temp<len;temp++)
    {
        USART_SendData(usart_handle->Usart,*dat);
	    while(USART_GetFlagStatus(usart_handle->Usart,USART_FLAG_TXE) == RESET);
    }
    return 0;
}

/**************************************************************************
 * 功能:使用串口发送(DMA发送)
 * 参数:
 * usart_handle:串口句柄地址 -> 见bsp_usart.h
 * dat:要发送的数据的地址
 * len:要清除的长度
 * 返回值:
 * 0:发送成功
 * 1:串口被DMA占用或其他原因,发送失败
 * 2022/3/11   庞碧璋
 ***********************************************************************/
uint8_t Usart_DMASend(USER_USART_Type*usart_handle,uint8_t *dat,uint8_t len)
{
    if(usart_handle->DMATx_Ch == NULL)
        return 1;
    if( usart_handle->TxSbuffer_Len == 0  )
    {
        uint8_t temp;
        usart_handle->TxSbuffer_Len = len;
        if(dat != usart_handle->TxSbuffer)
        {
            for(temp=0;temp<len;temp++)
            usart_handle->TxSbuffer[temp] = dat[temp];
        }
        usart_handle->DMATx_Ch->CNDTR = len;
        usart_handle->DMATx_Ch->CCR |= DMA_CCR1_EN;
        //TargetDMA_Channel[USARTx-1]->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}

/**************************************************************************
 * 功能:使用串口发送(DMA发送)字符串
 * 参数:
 * usart_handle:串口句柄地址 -> 见bsp_usart.h
 * str:要发送的字符串(一定要'\0'结尾)
 * 返回值:
 * 0:发送成功
 * 1:串口被DMA占用或其他原因,发送失败
 * 2022/3/11   庞碧璋
 ***********************************************************************/
uint8_t Usart_DMASendString(USER_USART_Type*usart_handle,uint8_t*str)
{
    if(usart_handle->DMATx_Ch == NULL)
        return 1;
    if( usart_handle->TxSbuffer_Len == 0 )
    {
        while(*str!='\0' && usart_handle->TxSbuffer_Len < usart_handle->TxSbuffer_MaxLen)
        {
            usart_handle->TxSbuffer[ usart_handle->TxSbuffer_Len ] = *str;
            usart_handle->TxSbuffer_Len++;
            str++;
        }
        usart_handle->DMATx_Ch->CNDTR = usart_handle->TxSbuffer_Len;
        usart_handle->DMATx_Ch->CCR |= DMA_CCR1_EN;
        return 0;
    }
    return 1;
}

/**************************************************************************
 * 功能:将一个字节载入串口接收缓存区
 * 参数:
 * usart_handle:串口句柄地址 -> 见bsp_usart.h
 * dat:字节
 * 返回值:无
 * 备注:别调用!由接收ISR调用
 * 2022/3/11   庞碧璋
 ***********************************************************************/
void Rx_SbufferInput(USER_USART_Type*usart_handle,uint8_t dat)
{
    //判断缓存区是否满载
    if(usart_handle->RxSbuffer_Len < usart_handle->RxSbuffer_MaxLen)
    {
        usart_handle->RxSbuffer[ usart_handle->RxSbuffer_Len ] = dat;
    }
    usart_handle->RxSbuffer_Len++;
}

int fputc (int c, FILE *fp)
{
    while( Usart_DMA_BusyCheck( usart1 ) ) ;
	USART_SendData(USART1,c);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	return c;
}

/******************************ISR**************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(&usart1,USART_ReceiveData(USART1));
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}


void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(&usart2,USART_ReceiveData(USART2));
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}

void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
    {
        Rx_SbufferInput(&usart3,USART_ReceiveData(USART3));
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
    {
        usart1.TxSbuffer_Len = 0;
        usart1.DMATx_Ch->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC7) == SET)
    {
        usart2.TxSbuffer_Len = 0;
        usart2.DMATx_Ch->CCR &= (uint16_t)(~DMA_CCR1_EN);
        DMA_ClearITPendingBit(DMA1_IT_TC7);
    }
}
