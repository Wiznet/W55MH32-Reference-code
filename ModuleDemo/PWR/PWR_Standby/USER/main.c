#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

void    UART_Configuration(uint32_t bound);
void    GPIO_Configuration(void);
uint8_t GetCmd(void);

int main(void)
{
    RCC_ClocksTypeDef clocks;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("PWR Standby Test.\n");
    printf("Enable WakeUp Pin - PA0\n");
    printf("Please Input 's', Come Standby Mode\n");

    PWR_WakeUpPinCmd(ENABLE);

    while (GetCmd() != 's');
    GPIO_Configuration();
    PWR_EnterSTANDBYMode();

    while (1);
}

void GPIO_Configuration(void)
{
    uint8_t i;

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    for (i = 0; i < (GPIOG_BASE - GPIOA_BASE) / 0x400; i++)
    {
        GPIO_Init((GPIO_TypeDef *)((APB2PERIPH_BASE + (i + 3) * 0x0400)), &GPIO_InitStructure);
    }

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
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

uint8_t GetCmd(void)
{
    uint8_t tmp = 0;

    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
    {
        tmp = USART_ReceiveData(USART1);
    }
    return tmp;
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

