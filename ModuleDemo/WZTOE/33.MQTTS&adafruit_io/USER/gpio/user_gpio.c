#include <stdio.h>
#include "user_gpio.h"

void     user_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LED_PIN | BEEP_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED_PORT, LED_PIN);

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
