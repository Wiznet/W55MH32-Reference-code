#include "key.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_PERIPH | KEY2_PERIPH, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = KEY1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY1_GROUP, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = KEY2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY2_GROUP, &GPIO_InitStructure);
}

uint8_t KEY_Scan(void)
{
    if (KEY1 == 0 || KEY2 == 0)
    {
        delay_ms(10); //de-dithering
        if (KEY1 == 0)
            return KEY1_PRES;
        else if (KEY2 == 0)
            return KEY2_PRES;
    }
    return 0;
}
