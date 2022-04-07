#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "self_stm32f10x.h"
#include <stdio.h>

/*************************************************
 * 作者: 庞碧璋
 * Github: https://github.com/CodingBugStd
 * csdn:   https://blog.csdn.net/RampagePBZ
 * Encoding: utf-8
 * create date: 2021/6/22
 * last date:   2021/10/1
 * 
 * 电赛的更改:关闭了串口的接收DMA 3/28
 * 
 * 例:
 * 发送1:
 * uint8_t test[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
 * Usart_Send(&User_Usart1,test,6);    //Usart_Send有返回值 0:发送成功 1:失败,这个串口的DMA在忙
 * 
 * 发生2:   使用DMA发生,不占用时序 -> dat参数可以是user_Usart.TxSbuffer
 * uint8_t test[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
 * Usart_DMASend(&User_Usart1,test,6); //Usart_DMASend有返回值 0:发送成功 1:失败,这个串口的DMA在忙
 * 
 * 接收:
 * //接收由ISR自动将字节放入缓存区中,直接通过句柄查询接收缓存区和接收长度即可
 * //需要达到某个条件执行某段程序自己去ISR里改
 * if(User_Usart1.RxSbuffer_Len!=0)
 * {
 *  //处理 User_Usart1.RxSbuffer[];
 * }
 * 
 * 
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	BSP_Usart_Init();
	Usart_DMASendString(&User_Usart1,"HelloWrold!");

	while(1)
	{
		soft_delay_ms(1000);
		if(User_Usart1.RxSbuffer_Len!=0)    //判断是否接收到了数据
		{
			Usart_DMASend(&User_Usart1,User_Usart1.RxSbuffer,User_Usart1.RxSbuffer_Len);    //将接收缓存发回去
			Usart_RxSbuffer_Clear(User_Usart1); //清除接收缓存
		}
	}
}
*************************************************/

typedef struct
{
    USART_TypeDef*Usart;        //串口
    DMA_Channel_TypeDef*DMATx_Ch;   //串口发送DMA通道 -> 不使用直接赋值NULL
    uint8_t RxSbuffer_Len;      //当前接收缓存区中的数据长度
    uint8_t TxSbuffer_Len;      //当前发送缓存区中的数据长度 -> 仅在使能了发送DMA下有效 -> 同时有串口在忙软件标志位的作用,由ISR清零和函数赋值,别管就行
    uint8_t RxSbuffer_MaxLen;   //定义接收缓存区最大长度
    uint8_t TxSbuffer_MaxLen;   //定义发送缓存区最大长度 -> 仅在使能了发送DMA下有效
    uint8_t*RxSbuffer;
    uint8_t*TxSbuffer;
}USER_USART_Type;

//通过下面这三个变量操作串口!

//建议别自己给成员赋值,全权交给函数和宏
extern USER_USART_Type usart1;
extern USER_USART_Type usart2;
extern USER_USART_Type usart3;

//直接传User_Usartx进来
#define Usart_RxSbuffer_Clear(handle)   handle.RxSbuffer_Len = 0    //直接传,不要用指针
#define Usart_DMA_BusyCheck(handle)     handle.TxSbuffer_Len == 0 ? 0 : 1      //0:空闲 1:在忙 ->由DMA的中断服务函数清零

void BSP_Usart_Init(void);
void USART_GPIO_Init(void);
void USART_Config(void);
void USART_NVIC_Config(void);
void USART_DMA_Config(void);

uint8_t*Usart_Read(uint8_t USARTx);
uint8_t Usart_RxCopy(uint8_t USARTx,uint8_t*buf,uint8_t len);
uint8_t Usart_Send(USER_USART_Type*usart_handle,uint8_t *dat,uint8_t len);
uint8_t Usart_DMASend(USER_USART_Type*usart_handle,uint8_t *dat,uint8_t len);
uint8_t Usart_DMASendString(USER_USART_Type*usart_handle,uint8_t*str);
void USART_RxSbuffer_Push(USER_USART_Type*usart_handle,uint8_t len);

void Rx_SbufferInput(USER_USART_Type*usart_handle,uint8_t dat);

//printf()重定向
int fputc (int c, FILE *fp);

#endif
