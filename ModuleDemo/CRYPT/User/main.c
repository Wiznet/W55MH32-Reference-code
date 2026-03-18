#include "define.h"
#include "w55mh32.h"
#include "wiz_bignum.h"
#include "wiz_bignum_tool.h"
#include "wiz_crypt_version.h"
#include "wiz_rand.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

uint8_t u8RngBuff[0x60] = {0};

void main(int args) {
  int i = 0, temp;
  uint32_t ver = 1;
  RCC_ClocksTypeDef clocks;

  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  RCC_WaitForHSEStartUp();

  RCC_PLLCmd(DISABLE);
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  RCC_PLLCmd(ENABLE);

  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    ;
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

  WIZRAND_Init((uint32_t *)&u8RngBuff, sizeof(u8RngBuff));

  ver = wiz_crypt_version();
  DBG_PRINT("WIZnet CARD Secure Test Demo V1.0, secure lib version is "
            "V%02x.%02x.%02x.%02x\n",
            ver >> 24, (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);
  DBG_PRINT("WIZnet CARD Crypt Test V1.0 start......\r\n");

  while (1) {
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

void NVIC_Configuration(void) {
#if USE_SPI_PRINT
  NVIC_DisableIRQ(SPI_IRQn);
  NVIC_SetPriority(SPI_IRQn, 0);
  NVIC_EnableIRQ(SPI_IRQn);
#endif
}

#if defined(__GNUC__)
int _write(int fd, char *pBuffer, int size) {
  int i = 0;

  for (i = 0; i < size; i++) {
    if ('\n' == pBuffer[i]) {
      SER_PutChar('\r');
    }
    SER_PutChar(pBuffer[i]);
  }

  return size;
}

int printf_GCC(char *fmt, ...) {
  int ret;
  va_list varg;

  va_start(varg, fmt);
  ret = vprintf(fmt, varg);
  va_end(varg);

  fflush(stdout);

  return ret;
}

#elif defined(__ICCARM__) || defined(__CC_ARM)
#endif

void UART_Configuration(uint32_t bound) {
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,
                         ENABLE); // 使能USART1，GPIOA时钟

  // USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);          // 初始化GPIOA.9

  // USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;            // PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化GPIOA.10

  USART_InitStructure.USART_BaudRate = bound; // 串口波特率
  USART_InitStructure.USART_WordLength =
      USART_WordLength_8b; // 字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;    // 无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None; // 无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式

  USART_Init(USART1, &USART_InitStructure); // 初始化串口1
  USART_Cmd(USART1, ENABLE);                // 使能串口1
}

int SER_PutChar(int ch) {
  while (!USART_GetFlagStatus(USART1, USART_FLAG_TC))
    ;
  USART_SendData(USART1, (uint8_t)ch);

  return ch;
}

int fputc(int c, FILE *f) {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  if (c == '\n') {
    SER_PutChar('\r');
  }
  return (SER_PutChar(c));
}

void printf_monobit(void) {
  uint32_t i, j;
  uint32_t t;
  int32_t sum;
  for (i = 0; i <= 0xff; i++) {
    sum = 0;
    for (j = 0; j < 8; j++) {
      t = (i >> j) & 0x01;
      sum += 2 * (int)t - 1;
    }
    DBG_PRINT("%3d,", sum);
    if (15 == i % 16)
      DBG_PRINT("\n");
  }
}

void printf_byte_bit(void) {
  uint32_t i, j;
  uint32_t t;
  int32_t sum;
  for (i = 0; i <= 0xff; i++) {
    sum = 0;
    for (j = 0; j < 8; j++) {
      t = (i >> j) & 0x01;
      sum += t;
    }
    DBG_PRINT("%3d,", sum);
    if (15 == i % 16)
      DBG_PRINT("\n");
  }
}

void printf_byte_bit_runs(void) {
  uint32_t i, j;
  int32_t sum;
  for (i = 0; i <= 0xff; i++) {
    sum = 0;
    for (j = 0; j < 7; j++) {
      if ((i >> j & 0x03) == 0x01 || ((i >> j & 0x03) == 0x02))
        sum++;
    }
    DBG_PRINT("%3d,", sum);
    if (15 == i % 16)
      DBG_PRINT("\n");
  }
}

void printf_long_runs_max(void) {
  uint32_t i, j;
  int32_t sum;
  int32_t max;
  for (i = 0; i <= 0xff; i++) {
    sum = 0;
    max = 0;
    for (j = 0; j < 8; j++) {
      if (i >> j & 0x01) {
        sum++;
        if (sum > max)
          max = sum;
      } else
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* User can add his own implementation to report the file name and line
   number, ex: DBG_PRINT("Wrong parameters value: file %s on line %d\r\n", file,
   line) */

  /* Infinite loop */
  while (1) {
  }
}
#endif
