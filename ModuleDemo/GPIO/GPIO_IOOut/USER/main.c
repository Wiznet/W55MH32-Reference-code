#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


#define GPIO_GROUP_TEST GPIOB
#define GPIO_MODE_TEST  GPIO_Mode_Out_PP
#define GPIO_SPEED_TEST GPIO_Speed_50MHz
#define GPIO_PIN1_TEST  GPIO_Pin_0
#define GPIO_PIN2_TEST  GPIO_Pin_2
#define GPIO_PIN3_TEST  GPIO_Pin_3

USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);
void GPIO_Configuration(void);

int main(void)
{
    RCC_ClocksTypeDef clocks;
    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);
    printf("GPIO IO Out Tset.\n");
    GPIO_Configuration();

    while (1)
    {
        GPIO_SetBits(GPIO_GROUP_TEST, GPIO_PIN1_TEST);
        delay_ms(200);
        GPIO_SetBits(GPIO_GROUP_TEST, GPIO_PIN2_TEST);
        delay_ms(200);
        GPIO_SetBits(GPIO_GROUP_TEST, GPIO_PIN3_TEST);
        delay_ms(200);
        GPIO_ResetBits(GPIO_GROUP_TEST, GPIO_PIN1_TEST);
        delay_ms(200);
        GPIO_ResetBits(GPIO_GROUP_TEST, GPIO_PIN2_TEST);
        delay_ms(200);
        GPIO_ResetBits(GPIO_GROUP_TEST, GPIO_PIN3_TEST);
        delay_ms(200);
    }
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_PIN1_TEST | GPIO_PIN2_TEST | GPIO_PIN3_TEST;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_TEST;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_TEST;
    GPIO_Init(GPIO_GROUP_TEST, &GPIO_InitStructure);
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

