#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include <rtthread.h>

int main(void)
{
    while (1);
}

//add rtt usart1 auto init
int UART_Configuration(void)
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

    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    return 0;
}
INIT_BOARD_EXPORT(UART_Configuration);

//add cmd
//add get_chipid cmd
uint32_t ChipUniqueID[3];
#define get_chipid GetChipID
void GetChipID(void)
{
    ChipUniqueID[0] = *(volatile uint32_t *)(0x1FFFF7F0);
    ChipUniqueID[1] = *(volatile uint32_t *)(0x1FFFF7EC);
    ChipUniqueID[2] = *(volatile uint32_t *)(0x1FFFF7E8);
    rt_kprintf("\nChip ID is:0x%08X-%08X-%08X\n\n", ChipUniqueID[0], ChipUniqueID[1], ChipUniqueID[2]);
}
MSH_CMD_EXPORT(get_chipid, get 96_bit unique chip id);

//add get_clk cmd
uint32_t ChipUniqueID[3];
#define get_clk GetClk
void GetClk(void)
{
    RCC_ClocksTypeDef clocks;

    RCC_GetClocksFreq(&clocks);
    rt_kprintf("SYSCLK: %dHz, \nHCLK: %dHz, \nPCLK1: %dHz, \nPCLK2: %dHz \nADCCLK: %dHz\n",
               clocks.SYSCLK_Frequency, clocks.HCLK_Frequency,
               clocks.PCLK1_Frequency, clocks.PCLK2_Frequency, clocks.ADCCLK_Frequency);
}
MSH_CMD_EXPORT(get_clk, get sysclk);

