/**
******************************************************************************
* @file    			bsp_uart.c
* @version 			V1.0
* @date    			2024-08-21
* @brief 		    Debug serial port initialization and serial port redirection
*
* @company  		WIZnet
* @website  		https://wiznet.io/
* @gitee            https://gitee.com/wiznet-hk
* @github           https://github.com/Wiznet/
******************************************************************************
*/

#include <stdio.h>
#include "bsp_uart.h"
#include "w55mh32.h"
#include <rtthread.h>

/**
 * @brief   debug usart initialization
 * @param   none
 * @return  none
 */
int console_usart_init(void)
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

INIT_BOARD_EXPORT(console_usart_init);

int fputc(int c, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TC));
    USART_SendData(USART1, (uint8_t)c);
    return c;
}
