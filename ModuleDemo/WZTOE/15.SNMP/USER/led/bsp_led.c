#include "w55mh32.h"
#include "bsp_led.h"
#include <stdio.h>

void user_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    /* Configure RSTn pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD, GPIO_Pin_14);
}

void set_user_led_status(int32_t val)
{
    if (!val)
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_14);
    }
    else
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_14);
    }
}

void get_user_led_status(void *ptr, uint8_t *len)
{
    uint8_t led_status = 0;
    led_status         = GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_14);
    *len               = sprintf((char *)ptr, "USER LED[%s].", led_status ? "off" : "on");
}
