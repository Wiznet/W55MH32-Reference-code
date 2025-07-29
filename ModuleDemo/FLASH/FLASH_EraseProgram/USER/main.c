#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

#define FLASH_TEST_ADDR (0x0800F400) //32K

void UART_Configuration(uint32_t bound);
void SYSTICK_Reset(void);
void FLASH_EraseProgram(uint32_t number);

int main(void)
{
    uint32_t number = 0;

    RCC_ClocksTypeDef clocks;
    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);
    printf("FLASH Erase Program Tset.\n");

    while (1)
    {
        FLASH_EraseProgram(number++);
        if (number == 0xFFFFFFFF)
        {
            printf("Test End\n");
            while (1);
        }
    }
}

void SYSTICK_Reset(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
}

void FLASH_EraseProgram(uint32_t number)
{
    uint8_t  buff[1024] = {0};
    uint32_t status     = 0, value, i;

    FLASH_Unlock();

    memset(buff, 0xFF, sizeof(buff));

    SYSTICK_Reset();
    FLASH_ErasePage(FLASH_TEST_ADDR);
    status = FLASH_GetBank1Status();
    while (status == FLASH_BUSY)
    {
        status = FLASH_GetBank1Status();
    }
    value = SysTick->VAL;

    if (strncmp((char *)FLASH_TEST_ADDR, (char *)buff, 1024) == 0)
    {
        printf("%d, --> Erase Page Time is %d ms\n", number, (0xFFFFFF - value) / 72000);
    }
    else
    {
        printf("Erase Page Fail\n");
        while (1);
    }

    SYSTICK_Reset();
    for (i = 0; i < 1024 / 2; i++)
    {
        FLASH_ProgramHalfWord(FLASH_TEST_ADDR + (2 * i), 0x5A5A);
        while (status == FLASH_BUSY)
        {
            status = FLASH_GetBank1Status();
        }
    }
    value = SysTick->VAL;

    memset(buff, 0x5A, sizeof(buff));
    if (strncmp((char *)FLASH_TEST_ADDR, (char *)buff, 1024) == 0)
    {
        printf("%d, --> Program Half Word is %d ms\n", number, (0xFFFFFF - value) / 72000);
    }
    else
    {
        printf("Program Half Word Fail\n");
        while (1);
    }

    FLASH_Lock();
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

