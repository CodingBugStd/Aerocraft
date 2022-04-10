#include "bsp_key.h"

static const Pin KEY_Pin[4] = {
    {GPIO_Pin_11,GPIOE},
    {GPIO_Pin_12,GPIOE},
    {GPIO_Pin_13,GPIOE},
    {GPIO_Pin_14,GPIOE}
};

void BSP_Key_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Pin = 0x000F<<11;     //Pin11 ~ Pin14
    
    GPIO_Init(GPIOE,&GPIO_InitStruct);
}

uint8_t Key_Read(uint8_t key)
{
    if(Pin_Read(KEY_Pin[key]))
        return 0;
    else
        return 1;
}
