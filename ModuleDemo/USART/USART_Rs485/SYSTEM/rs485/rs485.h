#ifndef __RS485_H
#define __RS485_H
#include "w55mh32.h"

extern uint8_t RS485_RX_BUF[64]; //Receive buffer, up to 64 bytes
extern uint8_t RS485_RX_CNT;     //Length of data received

#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
//IO port address mapping
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) //0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) //0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) //0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) //0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) //0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) //0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) //0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8)  //0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8)  //0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8)  //0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8)  //0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8)  //0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8)  //0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8)  //0x40011E08

//IO port operation, only for a single IO port!
//Make sure the value of n is less than 16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n)

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n)

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n)

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n)

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n)
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n)

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n)
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n)


//mode control
#define RS485_TX_EN PCout(4) //485 mode control. 0, receive; 1, send.
//If you want to interrupt the serial port, please do not comment on the following macro definition
#define EN_USART3_RX 1 //0, do not accept; 1, accept.

void RS485_Init(uint32_t bound);
void RS485_Send_Data(uint8_t *buf, uint8_t len);
void RS485_Receive_Data(uint8_t *buf, uint8_t *len);

#endif

