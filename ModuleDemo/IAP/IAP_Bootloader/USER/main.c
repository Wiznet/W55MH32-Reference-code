#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include "key.h"
#include "iap.h"


USART_TypeDef *USART_TEST = USART1;
#define USART_REC_LEN 32 * 1024 //Define the maximum number of received bytes 32K

void UART_Configuration(uint32_t bound);
void NVIC_Configuration(void);

uint16_t USART_RX_STA = 0; //receive status flag
uint16_t USART_RX_CNT = 0; //Number of bytes received
uint8_t  USART_RX_BUF[USART_REC_LEN] __attribute__((at(0X20001000)));

int main(void)
{
    uint8_t  key;
    uint16_t oldcount = 0; //The old serial port receives the data value.
    uint16_t applenth = 0; //The length of the received app code

    RCC_ClocksTypeDef clocks;

    delay_init();
    UART_Configuration(115200);
    NVIC_Configuration();
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("IAP Bootloader Test.\n");
    printf("KEY1:Copy To FLASH\n");
    printf("KEY2:Run FLASH APP\n");

    KEY_Init();
    while (1)
    {
        if (USART_RX_CNT)
        {
            if (oldcount == USART_RX_CNT)
            {
                applenth     = USART_RX_CNT;
                oldcount     = 0;
                USART_RX_CNT = 0;
                printf("User program reception complete!\r\n");
                printf("code length:%dBytes\r\n", applenth);
            }
            else
            {
                oldcount = USART_RX_CNT;
            }
        }

        delay_ms(100);

        key = KEY_Scan();
        if (key == KEY1_PRES)
        {
            if (applenth)
            {
                printf("Start updating firmware...\r\n");
                printf("Copying To FLASH...\n");
                if (((*(vu32 *)(0X20001000 + 4)) & 0xFF000000) == 0x08000000)  //Determine whether it is 0X08XXXXXX.
                {
                    IAP_Write_Appbin(FLASH_APP1_ADDR, USART_RX_BUF, applenth); //Update Flash Code
                    printf("Copy APP Successed!!\n");
                    printf("Firmware update complete!\r\n");
                }
                else
                {
                    printf("Illegal FLASH APP!  \n");
                    printf("Non-flash applications!\r\n");
                }
            }
            else
            {
                printf("There is no firmware that can be updated!\r\n");
                printf("No APP!\n");
            }
        }

        if (key == KEY2_PRES)
        {
            printf("Start executing FLASH user code!!\r\n");
            if (((*(vu32 *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000) //Determine whether it is 0X08XXXXXX.
            {
                IAP_Load_App(FLASH_APP1_ADDR);                                 //Execute FLASH APP code
            }
            else
            {
                printf("Non-FLASH applications cannot be executed!\r\n");
                printf("Illegal FLASH APP!\n");
            }
        }
    }
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    USART_ITConfig(USART_TEST, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART_TEST, ENABLE);
}

void USART1_IRQHandler(void)
{
    uint8_t res;
    if (USART_GetFlagStatus(USART_TEST, USART_FLAG_RXNE))
    {
        res = USART_ReceiveData(USART_TEST);
        if (USART_RX_CNT < USART_REC_LEN)
        {
            USART_RX_BUF[USART_RX_CNT] = res;
            USART_RX_CNT++;
        }
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

