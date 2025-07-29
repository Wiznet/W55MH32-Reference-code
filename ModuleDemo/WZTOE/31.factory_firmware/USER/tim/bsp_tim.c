/**
******************************************************************************
* @file    			bsp_tim.c
* @version 			V1.0
* @date    			2024-08-21
* @brief 		    wizchip tim initialization
*
* @company  		WIZnet
* @website  		https://wiznet.io/
* @gitee            https://gitee.com/wiznet-hk
* @github           https://github.com/Wiznet/
******************************************************************************
*/

#include "w55mh32.h"
#include "bsp_tim.h"
#include "wiz_interface.h"
#include "dhcp.h"
#include "httpServer.h"
#include "aht20.h"
extern MYI2C_Struct SENx;
extern uint32_t     btn_timer_cnt;
extern uint8_t      btn_timer_flag;
extern uint16_t     btn_cnt;
/**
 * @brief   1ms timer init
 * @param   none
 * @return  none
 */
void tim3_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    RCC_ClocksTypeDef       clocks;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // Clock enablement
    RCC_GetClocksFreq(&clocks);

    TIM_TimeBaseStructure.TIM_Period        = 1000 - 1;                            // Automatic reloading cycle value
    TIM_TimeBaseStructure.TIM_Prescaler     = clocks.HCLK_Frequency / 1000000 - 1; // Clock frequency pre division value
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;                  // Up counting mode
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // Enable update interrupt

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief   1ms timer IRQ Handler
 * @param   none
 * @return  none
 */
void TIM3_IRQHandler(void)
{
    static uint32_t tim3_1ms_count   = 0;
    static uint8_t  tim3_100ms_count = 0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        tim3_1ms_count++;
        tim3_100ms_count++;
        btn_timer_cnt++;
        if (btn_timer_flag && btn_timer_cnt >= 20)
        {
            btn_timer_flag = 0;
            if ((GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_5) == RESET))
            {
                btn_cnt++;
            }
        }
        if (tim3_100ms_count >= 100)
        {
            tim3_100ms_count = 0;
            MYI2C_Handle(&SENx);
        }
        if (tim3_1ms_count >= 1000)
        {
            DHCP_time_handler();
            httpServer_time_handler();
            tim3_1ms_count = 0;
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}
