#include "delay.h"
#include "w55mh32.h"

#define MAX_LOAD 0xFFFFFF

static uint8_t  UsNumber = 0;
static uint16_t MsNumber = 0;
static uint32_t max_us   = 0;

void delay_init()
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    UsNumber = clocks.SYSCLK_Frequency / 8 / 1000000;
    MsNumber = (u16)UsNumber * 1000;
    max_us   = MAX_LOAD / UsNumber;
}

void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD  = nus * UsNumber;
    SysTick->VAL   = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL   = 0X00;
}

void delay_ms(u16 nms)
{
    u32 temp;
    do
    {
        SysTick->LOAD  = (nms * 1000 > max_us) ? (max_us * UsNumber) : (((u32)nms * MsNumber));
        nms           -= (SysTick->LOAD / MsNumber);
        SysTick->VAL   = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do
        {
            temp = SysTick->CTRL;
        } while ((temp & 0x01) && !(temp & (1 << 16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL   = 0X00;

    } while (nms > 0);
}
