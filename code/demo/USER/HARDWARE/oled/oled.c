#include "oled.h"
#include "oled_font.h"

#define IIC_Start()		I2C_Start(OLED_IIC)
#define IIC_WaitACK()	I2C_NoWaitACK(OLED_IIC)
#define	IIC_SendByte(dat)	I2C_SendByte(OLED_IIC,dat)
#define IIC_Stop()		I2C_Stop(OLED_IIC)

static uint8_t oled_sbuffer[8][128];

static uint8_t initCmd[28] = 
{
	0xae,0x00,0x10,0x40,0x81,0xcf,
    0xa1,0xc8,0xa6,0xa8,0x3f,0xd3,
    0x00,0xd5,0x80,0xd9,0xf1,0xda,
    0x12,0xdb,0x40,0x20,0x02,0x8d,
    0x14,0xa4,0xa6,0xaf
};

void OLED_Init(void)
{
	soft_delay_ms(1000);	//根据20年代码改的!必须要延时!否则一上电iic刚刚初始化直接执行下面的代码会导致iic时序有点问题!!!
	#if 0
	OLED_SendByte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_SendByte(0x00,OLED_CMD);//---set low column address
	OLED_SendByte(0x10,OLED_CMD);//---set high column address
	OLED_SendByte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_SendByte(0x81,OLED_CMD);//--set contrast control register
	OLED_SendByte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_SendByte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0×óÓÒ·´ÖÃ 0xa1Õý³£
	OLED_SendByte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0ÉÏÏÂ·´ÖÃ 0xc8Õý³£
	OLED_SendByte(0xA6,OLED_CMD);//--set normal display
	OLED_SendByte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_SendByte(0x3f,OLED_CMD);//--1/64 duty
	OLED_SendByte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_SendByte(0x00,OLED_CMD);//-not offset
	OLED_SendByte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_SendByte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_SendByte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_SendByte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_SendByte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_SendByte(0x12,OLED_CMD);
	OLED_SendByte(0xDB,OLED_CMD);//--set vcomh
	OLED_SendByte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_SendByte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_SendByte(0x02,OLED_CMD);//
	OLED_SendByte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_SendByte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_SendByte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_SendByte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_SendByte(0xAF,OLED_CMD);
	#endif
	OLED_SendNumByte(initCmd,28,OLED_CMD);
	OLED_Clear();
}

void OLED_DisPlay_On(void)
{
	OLED_SendByte(0x8D,OLED_CMD);
	OLED_SendByte(0x14,OLED_CMD);
	OLED_SendByte(0xAF,OLED_CMD);
}

void OLED_DisPlay_Off(void)
{
	OLED_SendByte(0x8D,OLED_CMD);
	OLED_SendByte(0x10,OLED_CMD);
	OLED_SendByte(0xAF,OLED_CMD);
}

void OLED_PosSet(uint8_t page,uint8_t col)
{
	if(page>7 || col > 127)
		return;
	OLED_SendByte(0xb0+page,OLED_CMD);
	OLED_SendByte(0x00+(col&0x0f),OLED_CMD);
	OLED_SendByte(0x10+((col&0xf0)>>4),OLED_CMD);
}

void OLED_ShowString(uint8_t*str,uint8_t page,uint8_t col,uint8_t size)
{
	uint8_t len;
	uint8_t Ncol;
	switch(size)
	{
		case 1:len=7;break;
		case 2:len=9;break;
		default:break;
	}
	for(Ncol=col;Ncol<127;Ncol+=len)
	{
		OLED_ShowChar(*str,page,Ncol,size);
		str++;
		if(*str=='\0')
			break;
	}
}

void OLED_ShowNum(uint16_t num,uint8_t page,uint8_t col,uint8_t size)
{
	uint16_t NumLen,temp;
	uint8_t Ncol,len,chr;
	switch(size)
	{
		case 1:len=7;break;
		case 2:len=9;break;
		default:break;
	}
	temp = num;
	for(NumLen=10;temp!=0;NumLen*=10)
		temp/=10;
	NumLen/=100;
	for(Ncol=col;Ncol<127;Ncol+=len)
	{
		chr = (num/NumLen)%10;
		OLED_ShowChar(chr+0x30,page,Ncol,size);
		NumLen/=10;
		if(NumLen==0)
			break;
	}
}

void OLED_ClearPage(uint8_t page)
{
	for(uint8_t col=0;col<128;col++)
		oled_sbuffer[page][col] = 0x00;
}

void OLED_ClearBlok(uint8_t page1,uint8_t page2,uint8_t col1,uint8_t col2)
{
	#if 1
	uint8_t Tpage;
	uint8_t Tcol;
	for(Tpage=page1;Tpage<page2+1;Tpage++)
	{
		OLED_PosSet(Tpage,col1);
		#if 1
		for(Tcol=col1;Tcol<col2+1;Tcol++)
			OLED_SendByte(0x00,OLED_DATA);
		#endif
	}
	#endif
}

void OLED_ShowNumChar(uint8_t*str,uint8_t page,uint8_t col,uint8_t size,uint8_t num)
{
	uint8_t len;
	uint8_t Ncol;
	switch(size)
	{
		case 1:len=7;break;
		case 2:len=9;break;
		default:break;
	}
	for(Ncol=col;Ncol<127;Ncol+=len)
	{
		OLED_ShowChar(*str,page,Ncol,size);
		num--;
		if(num == 0)
			break;
	}
}

void OLED_Refresh(void)
{
	uint8_t page;
	for(page=0;page<8;page++)
	{
		OLED_PosSet(page,0);
		OLED_SendNumByte(oled_sbuffer[page],128,OLED_DATA);
	}
}

void OLED_ShowChar(uint8_t chr,uint8_t page,uint8_t col,uint8_t size)
{
	OLED_PosSet(page,col);
	if((chr>=' ')&&(chr<='z'))
	switch(size)
	{
		case 1:
			MemCopy(asc2_0806[chr-32],&oled_sbuffer[page][col],6);
			#if 0
			OLED_SendNumByte(asc2_0806[chr-32],6,OLED_DATA);
			#endif
			#if 0
			for(temp=0;temp<6;temp++)
				OLED_SendByte(asc2_0806[chr-32][temp],OLED_DATA);
			#endif
			break;
		case 2:
			MemCopy(asc2_1608[chr-32],&oled_sbuffer[page][col],8);
			MemCopy(&asc2_1608[chr-32][8],&oled_sbuffer[page+1][col],8);
			#if 0
			OLED_SendNumByte(asc2_1608[chr-32],8,OLED_DATA);
			OLED_PosSet(page+1,col);
			OLED_SendNumByte(&asc2_1608[chr-32][8],8,OLED_DATA);
			#endif
			#if 0
			for(temp=0;temp<8;temp++)
				OLED_SendByte(asc2_1608[chr-32][temp],OLED_DATA);
			OLED_PosSet(page+1,col);
			for(temp=0;temp<8;temp++)
				OLED_SendByte(asc2_1608[chr-32][temp+8],OLED_DATA);
			#endif
			break;
		default:break;
	}
}

void OLED_Clear(void)
{
	uint8_t page,col;
	for(page=0;page<8;page++)
	{
		OLED_PosSet(page,0);
		for(col=0;col<128;col++)
			OLED_SendByte(0x00,OLED_DATA);
	}
}

void OLED_SendByte(uint8_t dat,uint8_t Mode)
{
	IIC_Start();
	IIC_SendByte(0x78);
	IIC_WaitACK();
	if(Mode)
		IIC_SendByte(0x40);
	else
		IIC_SendByte(0x00);
	IIC_WaitACK();
	IIC_SendByte(dat);
	IIC_WaitACK();
	IIC_Stop();
}

void OLED_SendNumByte(uint8_t*dat,uint8_t len,uint8_t Mode)
{
	IIC_Start();
	IIC_SendByte(0x78);
	IIC_WaitACK();
	if(Mode)
		IIC_SendByte(0x40);
	else
		IIC_SendByte(0x00);
	IIC_WaitACK();
	for(uint8_t temp=0;temp<len;temp++)
	{
		IIC_SendByte(*dat);
		IIC_WaitACK();
		dat++;
	}
	IIC_Stop();
}
