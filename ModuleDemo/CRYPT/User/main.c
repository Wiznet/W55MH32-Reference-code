#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "w55mh32.h"
#include "wiz_bignum.h"
#include "wiz_rand.h"
#include "wiz_bignum_tool.h"
#include "wiz_crypt_version.h"
#include "define.h"

#define CRYPT_INT 0

#define WIZ_MAX_RSA_MODULUS_WORDS ((WIZ_MAX_RSA_MODULUS_BYTES + 3) / 4)
#define WIZ_MAX_RSA_PRIME_WORDS   ((WIZ_MAX_RSA_PRIME_BYTES + 3) / 4)

void SPI_Configuration();
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void SYSCLK_Config(void);

void UART_Configuration(uint32_t bound);

extern void AES_Test(void);
extern void DES_Test(void);
extern void TDES_Test(void);
extern void SM4_Test(void);
extern void SM3_Test(void);
extern void SHA_Test(void);
extern void SM1_Test(void);
extern void SM7_Test(void);


extern SPI_BUF  SpiSendBuf, SpiRecvBuf;
extern uint32_t gu32RecvESFlag, gu32SendESFlag;
uint8_t         u8RngBuff[0x60] = {0};

void main(int args)
{
    int               i               = 0, temp;
    uint32_t          ver             = 1;
    uint32_t          au32BusKey[2]   = {0};
    uint32_t          au32CramKey[2]  = {0};
    uint8_t           au8RngBuf[0x60] = {0};
    RCC_ClocksTypeDef clocks;

    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();

    RCC_PLLCmd(DISABLE);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    SysTick->CTRL |= 0x04;

    *(uint32_t *)(0x400210F0) = 0x01;
    *(uint32_t *)(0x40016C00) = 0xCDED3526;
    *(uint32_t *)(0x40016CCC) = 0x07;

    UART_Configuration(115200);

    RCC_GetClocksFreq(&clocks);
    DBG_PRINT("\n");
    DBG_PRINT("****SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz*****\n", (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
              (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);
    DBG_PRINT("***********************************W55MH32 Test!***********************************\n");
    DBG_PRINT("\n");

    WIZRAND_Init((uint32_t *)&u8RngBuff, sizeof(u8RngBuff));

    ver = wiz_crypt_version();
    DBG_PRINT("WIZnet CARD Secure Test Demo V1.0, secure lib version is V%02x.%02x.%02x.%02x\n", ver >> 24, (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);
    DBG_PRINT("WIZnet CARD Crypt Test V1.0 start......\r\n");

    while (1)
    {
        i++;
        DBG_PRINT("______round : %d_____\n", i);

        AES_Test();
        DES_Test();
        TDES_Test();
        SHA_Test();
        SM1_Test();
        SM3_Test();
        SM4_Test();
        SM7_Test();
    }
}

void NVIC_Configuration(void)
{
#if USE_SPI_PRINT
    NVIC_DisableIRQ(SPI_IRQn);
    NVIC_SetPriority(SPI_IRQn, 0);
    NVIC_EnableIRQ(SPI_IRQn);
#endif
}


#if defined(__GNUC__)
int _write(int fd, char *pBuffer, int size)
{
    int i = 0;

    for (i = 0; i < size; i++)
    {
        if ('\n' == pBuffer[i])
        {
            SER_PutChar('\r');
        }
        SER_PutChar(pBuffer[i]);
    }

    return size;
}

int printf_GCC(char *fmt, ...)
{
    int     ret;
    va_list varg;

    va_start(varg, fmt);
    ret = vprintf(fmt, varg);
    va_end(varg);

    fflush(stdout);

    return ret;
}

#elif defined(__ICCARM__) || defined(__CC_ARM)
#endif

void UART_Configuration(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //Enable USART1, GPIOA clock

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;      //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; //Multiplexed push-pull output
    GPIO_Init(GPIOA, &GPIO_InitStructure);           //Initialize GPIOA.9

    //USART1_RX	  GPIOA.10 initialization
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;                                     //PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                           //floating input
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                          //Initialize GPIOA.10

    USART_InitStructure.USART_BaudRate            = bound;                          //serial port baud rate
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;            //Word length is 8-bit data format
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;               //a stop position
    USART_InitStructure.USART_Parity              = USART_Parity_No;                //no parity check bit
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //no hardware data flow control
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;  //sending and receiving mode

    USART_Init(USART1, &USART_InitStructure);                                       //Initialize serial port 1
    USART_Cmd(USART1, ENABLE);                                                      //Enable serial port 1
}


int SER_PutChar(int ch)
{
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TC));
    USART_SendData(USART1, (uint8_t)ch);

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


void printf_monobit(void)
{
    uint32_t i, j;
    uint32_t t;
    int32_t  sum;
    for (i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for (j = 0; j < 8; j++)
        {
            t    = (i >> j) & 0x01;
            sum += 2 * (int)t - 1;
        }
        DBG_PRINT("%3d,", sum);
        if (15 == i % 16)
            DBG_PRINT("\n");
    }
}

void printf_byte_bit(void)
{
    uint32_t i, j;
    uint32_t t;
    int32_t  sum;
    for (i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for (j = 0; j < 8; j++)
        {
            t    = (i >> j) & 0x01;
            sum += t;
        }
        DBG_PRINT("%3d,", sum);
        if (15 == i % 16)
            DBG_PRINT("\n");
    }
}


void printf_byte_bit_runs(void)
{
    uint32_t i, j;
    int32_t  sum;
    for (i = 0; i <= 0xff; i++)
    {
        sum = 0;
        for (j = 0; j < 7; j++)
        {
            if ((i >> j & 0x03) == 0x01 || ((i >> j & 0x03) == 0x02))
                sum++;
        }
        DBG_PRINT("%3d,", sum);
        if (15 == i % 16)
            DBG_PRINT("\n");
    }
}


void printf_long_runs_max(void)
{
    uint32_t i, j;
    int32_t  sum;
    int32_t  max;
    for (i = 0; i <= 0xff; i++)
    {
        sum = 0;
        max = 0;
        for (j = 0; j < 8; j++)
        {
            if (i >> j & 0x01)
            {
                sum++;
                if (sum > max)
                    max = sum;
            }
            else
                sum = 0;
        }
        DBG_PRINT("%3d,", max);
        if (15 == i % 16)
            DBG_PRINT("\n");
    }
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: DBG_PRINT("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

