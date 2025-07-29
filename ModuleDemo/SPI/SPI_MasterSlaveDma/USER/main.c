#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include "wiz.h"

//This demo is used as the SPI host. SpiSlaveConfig is spiInitStructure. SPI_Mode = SPI_Mode_Master;

USART_TypeDef *USART_TEST = USART1;
void           UART_Configuration(uint32_t bound);

void RCC_ClkConfiguration(void);

/***********************************/
#define SPI_BUFF_SIZE 256
uint8_t SPI_TX_BUFF[SPI_BUFF_SIZE];
uint8_t SPI_RX_BUFF[SPI_BUFF_SIZE];

//Spix 1: use SPI1, pins PA4~ PA7
//Spix 2: use SPI3, pins PA15,PB3~ PB5
#define SPI_MASTER
//#define SPI_SLAVE

#ifdef SPI_SLAVE
#define UseSpi                   SPI1
#define UseTxChannel             DMA1_Channel3
#define UseRxChannel             DMA1_Channel2
#define UseRxIRQn                DMA1_Channel2_IRQn
#define UseSpiTransFinishHanlder DMA1_Channel2_IRQHandler
#define UseDmaFinishFlag         DMA1_FLAG_TC2
#endif

#ifdef SPI_MASTER
#define UseSpi                   SPI3
#define UseTxChannel             DMA2_Channel2
#define UseRxChannel             DMA2_Channel1
#define UseRxIRQn                DMA2_Channel1_IRQn
#define UseSpiTransFinishHanlder DMA2_Channel1_IRQHandler
#define UseDmaFinishFlag         DMA2_FLAG_TC1
#endif

volatile unsigned char SpiTransFinish = 0;

void SpiConfig(void);
void SpiDmaTrans(unsigned char *sendBuffer, unsigned char *reciveBuffer, unsigned int len);

/***********************************/

int main(void)
{
    RCC_ClocksTypeDef clocks;
    uint16_t          i;

    RCC_ClkConfiguration();

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

    while (1)
    {
        memset(SPI_RX_BUFF, 0, sizeof(SPI_RX_BUFF));
        SpiConfig();
        SpiDmaTrans(SPI_TX_BUFF, SPI_RX_BUFF, SPI_BUFF_SIZE);
        while (!SpiTransFinish);
        if (memcmp(SPI_RX_BUFF, SPI_TX_BUFF, SPI_BUFF_SIZE))
        {
            printf(">>error\r\n");
            printf("recv data:\n");
            for (i = 0; i < SPI_BUFF_SIZE; i++)
            {
                printf("0x%x	", SPI_RX_BUFF[i]);
                if (SPI_BUFF_SIZE % 12 == 0) printf("\n");
            }
            while (1);
        }
        else
            printf(">>success\r\n");
        delay_ms(10);
    }
}

/*
 *	void SpiConfig(void)
 *	Initialize SPI
 */
void SpiConfig(void)
{
    GPIO_InitTypeDef gpioInitStructure;
    SPI_InitTypeDef  spiInitStructure;
    DMA_InitTypeDef  dmaInitStructure;
    NVIC_InitTypeDef nvicInitStruct;

    //Turn on the required peripheral clock, and configure pin IO
#ifdef SPI_SLAVE
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    gpioInitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpioInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStructure);

    gpioInitStructure.GPIO_Pin   = GPIO_Pin_4;
    gpioInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStructure);
#endif

#ifdef SPI_MASTER
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    gpioInitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    gpioInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInitStructure);

    gpioInitStructure.GPIO_Pin   = GPIO_Pin_15;
    gpioInitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
#endif

    //DMA configuration
    DMA_DeInit(UseRxChannel);
    DMA_DeInit(UseTxChannel);
    dmaInitStructure.DMA_MemoryBaseAddr     = (uint32_t)NULL;
    dmaInitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    dmaInitStructure.DMA_BufferSize         = 0;
    dmaInitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dmaInitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaInitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaInitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaInitStructure.DMA_Mode               = DMA_Mode_Normal;
    dmaInitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    dmaInitStructure.DMA_M2M                = DMA_M2M_Disable;
    dmaInitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UseSpi->DR;
    DMA_Init(UseRxChannel, &dmaInitStructure);
    DMA_ITConfig(UseRxChannel, DMA_IT_TC, ENABLE);

    dmaInitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Init(UseTxChannel, &dmaInitStructure);

    //Interrupt configuration. Enable DMA to receive the complete interrupt.
    nvicInitStruct.NVIC_IRQChannel                   = UseRxIRQn;
    nvicInitStruct.NVIC_IRQChannelCmd                = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0x0;
    nvicInitStruct.NVIC_IRQChannelSubPriority        = 0x0;
    NVIC_Init(&nvicInitStruct);

    //SPI configuration
    spiInitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
#ifdef SPI_MASTER
    spiInitStructure.SPI_Mode = SPI_Mode_Master;
#endif
#ifdef SPI_SLAVE
    spiInitStructure.SPI_Mode = SPI_Mode_Slave;
#endif
    spiInitStructure.SPI_DataSize          = SPI_DataSize_8b;
    spiInitStructure.SPI_CPOL              = SPI_CPOL_Low;
    spiInitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    spiInitStructure.SPI_NSS               = SPI_NSS_Soft;
    spiInitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    spiInitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    spiInitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(UseSpi, &spiInitStructure);

    SPI_Cmd(UseSpi, ENABLE);
}

/*
* void SpiSlaveDmaTrans (unsigned char * sendBuffer, unsigned char * reciveBuffer, unsigned int len)
* Enable DMA to send and receive.
* sendBuffer: Send cache.
* reciveBuffer: Receive cache.
* len: Send & receive data length.
*/
void SpiDmaTrans(unsigned char *sendBuffer, unsigned char *reciveBuffer, unsigned int len)
{
#ifdef SPI_MASTER
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
#endif

    SPI_I2S_DMACmd(UseSpi, SPI_I2S_DMAReq_Tx, DISABLE);
    SPI_I2S_DMACmd(UseSpi, SPI_I2S_DMAReq_Rx, DISABLE);
    DMA_Cmd(UseTxChannel, DISABLE);
    DMA_Cmd(UseRxChannel, DISABLE);

    UseTxChannel->CMAR = (unsigned int)sendBuffer;
    UseRxChannel->CMAR = (unsigned int)reciveBuffer;

    UseTxChannel->CNDTR = len;
    UseRxChannel->CNDTR = len;

    SpiTransFinish = 0;

    DMA_Cmd(UseRxChannel, ENABLE);
    DMA_Cmd(UseTxChannel, ENABLE);
    SPI_I2S_DMACmd(UseSpi, SPI_I2S_DMAReq_Rx, ENABLE);
    SPI_I2S_DMACmd(UseSpi, SPI_I2S_DMAReq_Tx, ENABLE);
}

/*
* void UseSlaveSpiTransFinishHanlder (void)
Enable DMA receive complete interrupt.
Clear the DMA complete flag and set the flag used to inform the software that the DMA transfer is complete.
*/
void UseSpiTransFinishHanlder(void)
{
    if (DMA_GetFlagStatus(UseDmaFinishFlag))
    {
        DMA_ClearFlag(UseDmaFinishFlag);
        DMA_Cmd(UseTxChannel, DISABLE);
        DMA_Cmd(UseRxChannel, DISABLE);
        SpiTransFinish = 1;
#ifdef SPI_MASTER
        GPIO_SetBits(GPIOA, GPIO_Pin_15);
#endif
    }
}

void RCC_ClkConfiguration(void)
{
    RCC_DeInit();

    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    RCC_PLLCmd(DISABLE);
    WIZ_RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_27, 1);

    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_HSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
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

