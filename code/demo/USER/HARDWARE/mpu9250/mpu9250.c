#include "mpu9250.h"
#include "BSP\bsp_i2c.h"
#include "self_stm32f10x.h"

//#include "HARDWARE\oled\oled.h"

#define port_I2C_Start()	I2C_Start(MPU9250_IIC)
#define port_I2C_Stop()		I2C_Stop(MPU9250_IIC)
#define port_I2C_Send(dat)	I2C_SendByte(MPU9250_IIC,dat)
#define port_I2C_Read()		I2C_ReadByte(MPU9250_IIC)
#define port_I2C_WaitAck()	I2C_WaitAck(MPU9250_IIC)
#define port_I2C_Ack()		I2C_Ack(MPU9250_IIC)
#define port_I2C_NoAck()	I2C_Nack(MPU9250_IIC)

#define MPU9250_Addr	(0x50<<1)
#define I2C_WR	0
#define I2C_RD	1

uint8_t MPU9250_Check(void)
{
	uint8_t ack=0;
	port_I2C_Start();
	port_I2C_Send(MPU9250_Addr|I2C_WR);
	ack=port_I2C_WaitAck();
	port_I2C_Stop();
	return ack;
}

//MPU9250读取寄存器数据
//传入参数：data存放数据的长度的数组 len需要读取的数据长度 reg开始读取的寄存器
uint8_t MPU9250_Read_Reg(uint8_t *data,uint8_t len,uint8_t reg)
{
	uint8_t i=0,ret=0;
	
//	taskENTER_CRITICAL();           //进入临界区
//	{
		//发送写命令
		port_I2C_Start();
		port_I2C_Send(MPU9250_Addr|I2C_WR);
		if(port_I2C_WaitAck()==1)
		{
			ret=1;
			goto mpu9250_cmd_fail;
		}
		
		//发送寄存器地址
		port_I2C_Send(reg);	
		if(port_I2C_WaitAck()==1)
		{
			ret=2;
			goto mpu9250_cmd_fail;
		}
		
		//发送读命令
		port_I2C_Start();
		port_I2C_Send(MPU9250_Addr|I2C_RD);	
		if(port_I2C_WaitAck()==1)
		{
			ret=3;
			goto mpu9250_cmd_fail;
		}
		
		//读寄存器数据
		for(i=0;i<len;i++)
		{
			data[i]=port_I2C_Read();
			if(i<len-1)
				port_I2C_Ack();
			else	//最后一个字节发送非应答信号
				port_I2C_NoAck();
		}
mpu9250_cmd_fail:
		port_I2C_Stop();	//IIC发送停止信号

	//OLED_ShowNum(ret,4,0,1);	
//	}
//	taskEXIT_CRITICAL();            //退出临界区
	return ret;
}

//short CharToShort(unsigned char cData[])
//{
//	return ((short)cData[1]<<8)|cData[0];
//}

//MPU9250读取加速度数据
void MPU9250_Get_Accel(Accel_Struct *accel_struct)
{
	uint8_t accel_data[6];
	if(MPU9250_Read_Reg(accel_data,6,AX)==0)	//从MPU9250读取加速度数据
	{
		accel_struct->accel_x=((float)((short)((short)accel_data[1]<<8)|accel_data[0]))/32768*16;
		accel_struct->accel_y=((float)((short)((short)accel_data[3]<<8)|accel_data[2]))/32768*16;
		accel_struct->accel_z=((float)((short)((short)accel_data[5]<<8)|accel_data[4]))/32768*16;
	}
}

//MPU9250读取角速度数据
void MPU9250_Get_Angular_velocity(Angular_Velocity_Struct *angular_velocity_struct)
{
	uint8_t angular_data[6];
	if(MPU9250_Read_Reg(angular_data,6,GX)==0)	//从MPU9250读取角速度数据
	{
		angular_velocity_struct->gx=((float)((short)((short)angular_data[1]<<8)|angular_data[0]))/32768*2000;
		angular_velocity_struct->gy=((float)((short)((short)angular_data[3]<<8)|angular_data[2]))/32768*2000;
		angular_velocity_struct->gz=((float)((short)((short)angular_data[5]<<8)|angular_data[4]))/32768*2000;
	}
}

//MPU9250读取磁场数据
void MPU9250_Get_Magnetic_Field(Magnetic_Field_Struct *magnetic_field_srtuct)
{
	uint8_t magnetic_data[6];
	if(MPU9250_Read_Reg(magnetic_data,6,HX)==0)	//从MPU9250读取磁场数据
	{
		magnetic_field_srtuct->hx=((float)((short)((short)magnetic_data[1]<<8)|magnetic_data[0]));
		magnetic_field_srtuct->hy=((float)((short)((short)magnetic_data[3]<<8)|magnetic_data[2]));
		magnetic_field_srtuct->hz=((float)((short)((short)magnetic_data[5]<<8)|magnetic_data[4]));
	}
}

//MPU9250读取角度数据
void MPU9250_Get_Angle(Angle_Struct *angle_struct)
{
	uint8_t angle_data[6];
	if(MPU9250_Read_Reg(angle_data,6,Roll)==0)	//从MPU9250读取角度数据
	{
		angle_struct->roll=((float)((short)((short)angle_data[1]<<8)|angle_data[0]))/32768*180;
		angle_struct->pitch=((float)((short)((short)angle_data[3]<<8)|angle_data[2]))/32768*180;
		angle_struct->yaw=((float)((short)((short)angle_data[5]<<8)|angle_data[4]))/32768*180;
	}
}
