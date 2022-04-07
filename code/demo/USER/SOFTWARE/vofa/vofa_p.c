#include "vofa_p.h"

#define port_SendNumByte(dat,len)	Usart_Send(&usart3,dat,len)

static Frame Vofa_Sbuffer = {{0},{0x00,0x00,0x80,0x7f}};

void Vofa_Input(float data,unsigned char channel)
{
	if(channel < DataNum)
		Vofa_Sbuffer.Date[channel] = data;
}

void Vofa_Send(void)
{
	port_SendNumByte((uint8_t*)&Vofa_Sbuffer,sizeof(Frame));
	//Usart1_Send((uint8_t*)&Vofa_Sbuffer,sizeof(Frame));
}

