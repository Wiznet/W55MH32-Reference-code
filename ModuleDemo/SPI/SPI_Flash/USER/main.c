#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include "w25qxx.h"


USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);
void GPIO_Configuration(void);

//Array of strings to write to flash
const u8 TEXT_Buffer[] = {"WIZnet W55MH32 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)

int main(void)
{
    RCC_ClocksTypeDef clocks;
    uint32_t          FLASH_SIZE = 128 * 1024 * 1024; //FLASH size is 16M bytes
    uint8_t           datatemp[SIZE];

    delay_init();
    UART_Configuration(115200);

    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);
    printf("SPI FLAH Test.\n");

    W25QXX_Init();
    while (W25QXX_ReadID() != W25Q64) //W25Q128 not detected
    {
        printf("W25Q64 Check Failed!\n");
        printf("Read ID: 0x%x\n", W25QXX_ReadID());
        delay_ms(1000);
    }

    printf("W25Q64 Ready!\n");

    while (1)
    {
        printf("Start Write W25Q64....\n");
        W25QXX_Write((u8 *)TEXT_Buffer, FLASH_SIZE - 100, SIZE); //Write SIZE-length data starting at the 100th to last address
        printf("W25Q128 Write Finished!\n");                     //prompt that the delivery is complete.
        delay_ms(2000);
        printf("Start Read W25Q64.... \n");
        W25QXX_Read(datatemp, FLASH_SIZE - 100, SIZE); //From the 100th to the last address, read out SIZE bytes
        printf("The Data Readed Is:  \n");             //prompt that the delivery is complete.
        printf("%s\n", datatemp);
        delay_ms(2000);
    }
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

