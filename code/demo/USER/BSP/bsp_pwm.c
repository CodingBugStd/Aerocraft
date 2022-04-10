#include "bsp_pwm.h"

static TIM_TypeDef* Target_TIM[2] = {TIM4,TIM1};

static __IO uint16_t* PWM_Channel[6] = {
    &TIM4->CCR1,&TIM4->CCR2,&TIM4->CCR3,&TIM4->CCR4,
    &TIM1->CCR1,&TIM1->CCR2
};

static void PWM_GPIO_Init(void);
static void PWM_TIM_Init(void);
static void PWM_Ch_Init(void);

void BSP_PWM_Init(void)
{
    PWM_GPIO_Init();
    PWM_TIM_Init();
    PWM_Ch_Init();
    for(uint8_t temp = 0;temp<2;temp++)
        TIM_Cmd(Target_TIM[temp],ENABLE);
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    #if 0
    //开启1 4 8 定时器
    for(uint8_t temp = 0;temp<3;temp++)
        TIM_Cmd(Target_TIM[temp],ENABLE);
    //开启TIM 1 8高级定时器的PWM输出
    TIM_CtrlPWMOutputs(TIM1,ENABLE);
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    #endif
}

void PWM_GPIO_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    //TIM4
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB,&GPIO_InitStruct);
    //TIM1
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOE,&GPIO_InitStruct);
}

void PWM_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIMBase_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    //TIM4 TIM1 50Hz -> 电调 舵机
    TIMBase_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIMBase_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIMBase_InitStruct.TIM_Period = 20000 - 1;
    TIMBase_InitStruct.TIM_Prescaler = 72 -1 ;
    TIMBase_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1,&TIMBase_InitStruct);
    TIM_TimeBaseInit(TIM4,&TIMBase_InitStruct);
}

void PWM_Ch_Init(void)
{
    TIM_OCInitTypeDef   TIM_OCInitStruct;

    //电调:TIM4 OC1~OC4
    //舵机:TIM1 OC1
    TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;      //互补通道空闲电平
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low;         //互补通道有效电平
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;          //有效电平
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;   //不开启互补通道
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;

    TIM_OCInitStruct.TIM_Pulse = 1000 - 1;   //低位油门
    TIM_OC1Init(TIM4,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC2Init(TIM4,&TIM_OCInitStruct);
    TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC3Init(TIM4,&TIM_OCInitStruct);
    TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
    TIM_OC4Init(TIM4,&TIM_OCInitStruct);
    TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);

    TIM_OCInitStruct.TIM_Pulse = 1500 - 1;   //中位脉宽
    TIM_OC1Init(TIM1,&TIM_OCInitStruct);
    TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);

    //TIM1 OC2(互补) 舵机
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;    //互补高电平有效
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;    //驱动互补通道
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable; //不开启主通道
    TIM_OC1Init(TIM1,&TIM_OCInitStruct);
}

void PWM_Out(uint8_t Channel,uint16_t CCR)
{
    if(Channel < 6)
    *PWM_Channel[Channel] = CCR;
}

uint16_t PWM_Read(uint8_t Channel)
{
    if(Channel < 6)
        return *PWM_Channel[Channel];
    return 0xffff;
}
