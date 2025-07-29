#include <stdio.h>
#include "user_gpio.h"

uint16_t btn_cnt        = 0;
uint32_t btn_timer_cnt  = 0;
uint8_t  btn_timer_flag = 0;
void     user_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource5);

    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    EXTI_InitStructure.EXTI_Line    = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void led_on(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN);
}
void led_off(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN);
}
void beep_on(void)
{
    GPIO_SetBits(BEEP_PORT, BEEP_PIN);
}
void beep_off(void)
{
    GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) == SET)
    {
        if ((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5) == RESET))
        {
            btn_timer_flag = 1;
            btn_timer_cnt  = 0;
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
}
