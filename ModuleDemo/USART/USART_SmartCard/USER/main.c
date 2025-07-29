#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include "WIZ7816.h"


USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);


uint8_t  aucGetRes[]    = {0xA0, 0xC0, 0x00, 0x00, 0x00};
uint8_t  aucSeletMF[]   = {0xA0, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00};
uint8_t  aucResBuf[256] = {0};
uint16_t u16ResLen      = 0;
uint8_t  aucReqBuf[256] = {0};
uint16_t u16ReqLen      = 0;

void Smartcard_Test_Start(void);
void Smartcard_Test_Stop(void);
void Timer_Init(void);
void TIM_OFF(void);
void TIM_ON(void);
int  main(void)
{
    uint8_t ucFlag = 0;

    RCC_ClocksTypeDef clocks;

    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("USART Smart Card(T=0) Test.\n");
    Timer_Init();
    TIM_OFF();
    WIZ7816_Init();
    while (1)
    {
        if (GPIO_ReadInputDataBit(WIZ_OFFN_GPIOx, WIZ_OFFN_Pinx) && (ucFlag == 0))
        {
            printf("\r\n\r\nCard inserted\n\r");
            ucFlag = 1;
            Smartcard_Test_Start();
        }
        if ((!GPIO_ReadInputDataBit(WIZ_OFFN_GPIOx, WIZ_OFFN_Pinx)) && (ucFlag == 1))
        {
            printf("Card out\r\n\r\n");
            ucFlag = 0;
            Smartcard_Test_Stop();
        }
    }
}

void Smartcard_Test_Start(void)
{
    uint8_t  aucBuf[256] = {0};
    uint16_t u16Len      = 0;
    ColdReset(aucBuf, &u16Len);
    printf("ATR: ");
    if (u16Len == 0 || (aucBuf[0] == 0xFF) || (aucBuf[0] == 0x00))
        printf("ERR\r\n");
    else
    {
        for (uint8_t i = 0; i < u16Len; i++)
            printf("%02X ", aucBuf[i]);
        printf("\r\n");
        printf("Select MF: \r\n--> ");

        u16ReqLen = sizeof(aucSeletMF);
        memcpy(aucReqBuf, aucSeletMF, u16ReqLen);
        for (uint8_t i = 0; i < u16ReqLen; i++)
        {
            printf("%02X ", aucReqBuf[i]);
        }
        printf("\r\n");
        ExchangeTPDU(aucReqBuf, u16ReqLen, aucResBuf, &u16ResLen);

        if (u16ResLen == 0)
        {
            printf("No response/r/n");
            WIZ7816_SetVCC(0);
            WIZ7816_SetClkHz(0);
            return;
        }
        printf("<-- ");
        for (uint8_t i = 0; i < u16ResLen; i++)
        {
            printf("%02X ", aucResBuf[i]);
        }
        printf("\r\n");
        if (aucResBuf[0] == 0x9F)
        {
            aucGetRes[4] = aucResBuf[1];
            u16ReqLen    = sizeof(aucGetRes);
            memcpy(aucReqBuf, aucGetRes, u16ReqLen);

            printf("--> ");
            for (uint8_t i = 0; i < u16ReqLen; i++)
            {
                printf("%02X ", aucReqBuf[i]);
            }
            printf("\r\n");
            ExchangeTPDU(aucReqBuf, u16ReqLen, aucResBuf, &u16ResLen);
            if (u16ResLen == 0)
            {
                printf("No response/r/n");
                WIZ7816_SetVCC(0);
                WIZ7816_SetClkHz(0);
                return;
            }
            printf("<--");
            for (uint8_t i = 0; i < u16ResLen; i++)
            {
                printf("%02X ", aucResBuf[i]);
            }
            printf("\r\nSmartcard test OK!\r\n");
        }
        else
            printf("Cmd Err\r\n");
    }
    WIZ7816_SetVCC(0);
    WIZ7816_SetClkHz(0);
}

void Smartcard_Test_Stop(void)
{
    WIZ7816_SetVCC(0);
    WIZ7816_SetClkHz(0);
}

void UART_Configuration(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART_TEST, &USART_InitStructure);
    USART_Cmd(USART_TEST, ENABLE);
}
#define SMARTCARD_PRESCALER_SYSCLK 0X3
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;
    RCC_ClocksTypeDef       RCC_ClockState;
    uint32_t                tim_frequency = 0;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    RCC_GetClocksFreq(&RCC_ClockState);

    if (((RCC->CFGR & (uint32_t)0x700) >> 8) < 4)
        tim_frequency = RCC_ClockState.PCLK1_Frequency;
    else
        tim_frequency = RCC_ClockState.PCLK1_Frequency * 2;

    TIM_TimeBaseStructure.TIM_Prescaler   = tim_frequency / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Period      = 999;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;

    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM6, ENABLE);
}
void TIM_OFF(void)
{
    TIM_Cmd(TIM6, DISABLE);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM6->CNT = 0;
}


extern uint16_t WIZ_DelayMS;
void            TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        if (WIZ_DelayMS > 0)
            WIZ_DelayMS--;
        else
            TIM_OFF();
    }
}
int SER_PutChar(int ch)
{
    while (!USART_GetFlagStatus(USART_TEST, USART_FLAG_TC));
    USART_SendData(USART_TEST, (uint8_t)ch);

    return ch;
}

int fputc(int c, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    if (c == '\n')
    {
        SER_PutChar('\r');
    }
    return (SER_PutChar(c));
}

