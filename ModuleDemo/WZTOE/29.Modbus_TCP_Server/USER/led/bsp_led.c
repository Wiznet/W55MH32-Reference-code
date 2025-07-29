#include "w55mh32.h"
#include "bsp_led.h"
#include <stdio.h>

void user_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* Configure RSTn pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_SetBits(GPIOC, GPIO_Pin_4);
}

void set_user_led_status(uint32_t val)
{
    if (val)
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_4);
    }
}

int get_user_led_status(void)
{
    return GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_4);
}
