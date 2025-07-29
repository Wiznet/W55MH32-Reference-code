#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);
void GPIO_Configuration(void);
void SPI_Configuration(void);
void DMA_Configuration(void);

#define SPI_BUFF_SIZE 256
uint8_t SPI_TX_BUFF[SPI_BUFF_SIZE];
uint8_t SPI_RX_BUFF[SPI_BUFF_SIZE];


int main(void)
{
    RCC_ClocksTypeDef clocks;
    uint16_t          i;

    delay_init();
    UART_Configuration(115200);

    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);
    printf("SPI DMA Test.\n");

    for (i = 0; i < SPI_BUFF_SIZE; i++)
    {
        SPI_TX_BUFF[i] = i + 1;
    }
    memset(SPI_RX_BUFF, 0, sizeof(SPI_RX_BUFF));

    SPI_Configuration();
    DMA_Configuration();

    //Tx Data Test
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    DMA_Cmd(DMA1_Channel3, ENABLE);
    while (!DMA_GetFlagStatus(DMA1_FLAG_TC3));
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    delay_ms(100);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    while (1);
}

void DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA1 Channel2 (triggered by SPI1 Rx event) Config */
    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)SPI_RX_BUFF;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = SPI_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    /* Enable SPI1 DMA RX request */
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA1_Channel2, DISABLE);


    /* DMA1 Channel3 (triggered by SPI1 Tx event) Config */
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)SPI_TX_BUFF;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = SPI_BUFF_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);

    /* Enable SPI1 DMA TX request */
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
}

void SPI_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex; //Set SPI to one-way or two-way data mode: SPI is set to 2 lines full duplex
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                 //Set SPI working mode: Set the Master SPI
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                 //Set the data size of the SPI: SPI sends and receives 8-bit frame structures
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                   //The idle state of the serial synchronous clock is high
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                  //The second hop edge (up or down) of the serial synchronization clock is sampled
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                    //NSS signals are managed by hardware (NSS pins) or software (using SSI bits): Internal NSS signals are controlled by SSI bits
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;       //Define the value of the baud rate prescaler: The baud rate prescaler is 256.
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                //Specify whether the data transfer starts with the MSB bit or the LSB bit: Data transfer starts with the MSB bit
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                               //Polynomial for CRC Value Calculation
    SPI_Init(SPI1, &SPI_InitStructure);                                        //Initializes peripheral SPIx registers according to the parameters specified in the SPI_InitStruct

    SPI_Cmd(SPI1, ENABLE);                                                     //enable SPI peripherals
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

