#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);
void TIM_Configuration(void);

RCC_ClocksTypeDef clocks;
int               main(void)
{
    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("TIM Basic Test.\n");

    TIM_Configuration();

    while (1);
}

void TIM_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //clock enable

    //Timer TIM3 initialization
    TIM_TimeBaseStructure.TIM_Period        = 9999;                                 //Sets the value of the auto-reload register cycle for the next update event load activity
    TIM_TimeBaseStructure.TIM_Prescaler     = clocks.PCLK1_Frequency / 1000000 - 1; //Sets the prescaler value used as a divisor of the TIMx clock frequency
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                         //Set up clock splitting:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;                   //TIM Up Count Mode
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                                 //The time cardinality unit to initialize TIMx according to the specified parameters

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                                      //Enable the specified TIM3 interrupt, allow update interrupts

    //Interrupt Priority NVIC Settings
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn; //TIM3 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;         //Priority 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;         //From priority level 3
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    //The IRQ channel is enabled
    NVIC_Init(&NVIC_InitStructure);                                   //Initialize the NVIC register

    TIM_Cmd(TIM3, ENABLE);                                            //Enable TIMx
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

//Timer 3 interrupt service routine
void TIM3_IRQHandler(void)                             //TIM3 interrupt
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //Check if TIM3 update interruption occurred
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);    //Clear TIMx update interrupt flag
        printf("%s\n", __FUNCTION__);
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

