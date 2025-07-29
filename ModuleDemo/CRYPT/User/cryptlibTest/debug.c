#include <stdio.h>
#include "w55mh32.h"
#include "system_w55mh32.h"
#include "debug.h"


void r_printf(uint32_t b, char *s)
{
    if (0 != b)
    {
        DBG_PRINT("Pass ");
        DBG_PRINT("%s", s);
    }
    else
    {
        DBG_PRINT("Fail ");
        DBG_PRINT("%s", s);
        while (1);
    }
}


void ouputRes(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t  i;
    uint8_t *pu8Buff = pvbuff;

    DBG_PRINT("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        DBG_PRINT("%02X", pu8Buff[i]);
    }
    DBG_PRINT("\n");
}


void ouputArrayHex(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t  i;
    uint8_t *pu8Buff = pvbuff;

    DBG_PRINT("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        DBG_PRINT("0x%02X, ", pu8Buff[i]);
    }
    DBG_PRINT("\n");
}


volatile uint32_t gu32Tick = 0;

const uint32_t cau32TimRefClk[2] = {48000, 48000};


void timer_config(void)
{
    uint16_t                div, cnt;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure_APB1;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //clock enablement

    TIM_DeInit(TIM2);

    //1ms further interrupt
    div = cau32TimRefClk[0] / 1000 - 1;
    cnt = 999;

    TIM_TimeBaseStructure_APB1.TIM_Period        = cnt;                //set the value of the automatic reload register cycle for the next update event loading activity
    TIM_TimeBaseStructure_APB1.TIM_Prescaler     = div;                //set the pre division frequency value used as the divisor of TIMx clock frequency
    TIM_TimeBaseStructure_APB1.TIM_ClockDivision = TIM_CKD_DIV1;       //set clock segmentation:TDTS = Tck_tim
    TIM_TimeBaseStructure_APB1.TIM_CounterMode   = TIM_CounterMode_Up; //TIM Up Count Mode

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure_APB1);
}

void timer_init_ms(uint32_t ms)
{
    //    TIM_InitTypeDef s;

    //    //SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    //    //SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    //    NVIC->ICER[0] |= BIT(TIM0_IRQn);
    //    s.TIMx = TIM_0;
    //    s.TIM_Period = cau32TimRefClk[0 != (SYSCFG->CLK_CFG&SYSCTRL_TIM_REF_SEL_Mask)] * ms;

    //    TIM_DeInit(TIMM0);
    //    TIM_Cmd(TIMM0, TIM_0, DISABLE);
    //    TIM_Init(TIMM0, &s);
    //    TIM_ITConfig(TIMM0, TIM_0, ENABLE);
    //    gu32Tick = 0;
    //    TIM_Cmd(TIMM0, TIM_0, ENABLE);
    //    NVIC->ICPR[0] |= BIT(TIM0_IRQn);
    //    NVIC->ISER[0] |= BIT(TIM0_IRQn);

    NVIC_InitTypeDef NVIC_InitStructure;

    timer_config();

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;      //priority level 0 first
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;      //from priority level 3
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE; //The IRQ channel is enabled
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    gu32Tick = 0;
    TIM_Cmd(TIM2, ENABLE);
}
void time_avg(TIME_MS_US *pTimeTotal, uint32_t u32Count, TIME_MS_US *pTimeAvg)
{
    assert_param(NULL != pTimeTotal);
    assert_param(NULL != pTimeAvg);
    assert_param(0 != u32Count);

    pTimeAvg->u32Us = (pTimeTotal->u32Us + (pTimeTotal->u32Ms % u32Count) * 1000) / u32Count;
    pTimeAvg->u32Ms = pTimeTotal->u32Ms / u32Count;
}

void time_add(TIME_MS_US *pTime1, TIME_MS_US *pTime2, TIME_MS_US *pTimeSum)
{
    assert_param(NULL != pTime1);
    assert_param(NULL != pTime2);
    assert_param(NULL != pTimeSum);

    pTimeSum->u32Ms = pTime1->u32Ms + pTime2->u32Ms + (pTime1->u32Us + pTime2->u32Us) / 1000;
    pTimeSum->u32Us = (pTime1->u32Us + pTime2->u32Us) % 1000;
}


/**
 * Don't use this function in a interrupt which priority is less than timer tick.
 */

uint32_t get_time(TIME_MS_US *pTime)
{
    //    uint32_t u32Ms, u32Left, u32Tick, u32TimMs;
    //
    //    assert_param(NULL != pTime);
    //
    //    //TIM_Cmd(TIMM0, TIM_0, DISABLE);
    //
    //	TIM_Cmd(TIM2, DISABLE);
    //    u32Left = TIMM0->TIM[TIM_0].CurrentValue;
    //    u32Tick = gu32Tick;
    //    //TIM_Cmd(TIMM0, TIM_0, ENABLE);
    //
    //    u32TimMs = cau32TimRefClk[0 != (SYSCFG->CLK_CFG&SYSCTRL_TIM_REF_SEL_Mask)];
    //    //Left count less than a tick.
    //    u32Left = TIMM0->TIM[TIM_0].LoadCount - u32Left;
    //    //Get left ms
    //    u32Ms = u32Left / u32TimMs;
    //
    //    //Get left us
    //    pTime->u32Us = (u32Left % u32TimMs)  * 1000 / u32TimMs;

    //    //Get total ms(left ms + tick ms).
    //    pTime->u32Ms = u32Ms + TIMM0->TIM[TIM_0].LoadCount / u32TimMs * u32Tick;
    //
    //    TIM_Cmd(TIMM0, TIM_0, ENABLE);

    uint32_t u32Tick, u32Left;

    assert_param(NULL != pTime);

    u32Left = TIM2->CNT;
    u32Tick = gu32Tick;
    TIM_Cmd(TIM2, DISABLE);

    pTime->u32Ms = u32Tick;
    pTime->u32Us = u32Left;

    return 0;
}

#define PIN_NUM (4)
void timer_Verify()
{
    //    TIME_MS_US time[3];
    //    GPIO_RemapConfig(GPIO_Pin_4, GPIO_Remap_IO);
    //    GPIO->CON &= ~(0x03<<8);
    //    GPIO->CON |= 0x01<<8; // config gpio to input
    //    while(0 == (GPIO->IOD & BIT(PIN_NUM+8)));
    //    while(0 != (GPIO->IOD & BIT(PIN_NUM+8)));
    //    timer_init_ms(1);
    //    while(0 == (GPIO->IOD & BIT(PIN_NUM+8)));
    //    get_time(time);
    //    DBG_PRINT("%dms%dus ", time[0].u32Ms, time[0].u32Us);
}


//void TIM0_IRQHandler(void)
//{
//    TIM_ClearITPendingBit(TIMM0, TIM_0);
//    gu32Tick++;
//}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //Clear TIMx update interrupt flag
        gu32Tick++;
    }
}
