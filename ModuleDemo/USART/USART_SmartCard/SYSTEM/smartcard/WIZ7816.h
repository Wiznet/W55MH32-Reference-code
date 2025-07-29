#ifndef _WIZ7816_H
#define _WIZ7816_H

#include "w55mh32.h"

/* Pin definition */
#define WIZ_USARTx      USART2
#define WIZ_USARTx_IRQn USART2_IRQn

#define WIZ_CLK_GPIOx GPIOA
#define WIZ_CLK_Pinx  GPIO_Pin_4
#define WIZ_IO_GPIOx  GPIOA
#define WIZ_IO_Pinx   GPIO_Pin_2

#define WIZ_RST_GPIOx   GPIOC
#define WIZ_RST_Pinx    GPIO_Pin_8
#define WIZ_VCC_GPIOx   GPIOC
#define WIZ_VCC_Pinx    GPIO_Pin_6
#define WIZ_OFFN_GPIOx  GPIOC
#define WIZ_OFFN_Pinx   GPIO_Pin_7
#define WIZ_5V3VN_GPIOx GPIOC
#define WIZ_5V3VN_Pinx  GPIO_Pin_5


#define WIZ_RCC_APBxPeriphClockCmd                                                                      \
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); \
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)


extern uint8_t  ATR_TA1;   //The value of TA1 in the card ATR, TA1 contains FD
extern uint8_t  WIZ_T1;    //Whether the card is T = 1, 0 is T = 0, 1 is T = 1
extern uint32_t WIZ_WT;    //Communication timeout time WT = 9600

extern uint8_t  WIZ_F;     //F=1
extern uint8_t  WIZ_D;     //D=1
extern uint32_t WIZ_ClkHz; //frequency=3.6MHz


void WIZ7816_Init(void);                        //initialization
void WIZ7816_TIMxInt(void);                     //1MS regular interrupt call, providing time basis for communication timeout

void WIZ7816_SetClkHz(uint32_t hz);             //Set the clock frequency
void WIZ7816_SetFD(uint8_t F, uint8_t D);       //Set FD
void WIZ7816_SetWT(uint32_t wt);                //Set communication timeout
void WIZ7816_SetVCC(uint8_t on);                //Set up VCC

uint8_t ColdReset(uint8_t *atr, uint16_t *len); //cold reset
uint8_t WarmReset(uint8_t *atr, uint16_t *len); //thermal reset
uint8_t PPS(uint8_t F, uint8_t D);              //PPS
                                                //Send APDU and receive return data without error retransmission mechanism
uint8_t ExchangeTPDU(uint8_t *sData, uint16_t len_sData, uint8_t *rData, uint16_t *len_rData);

#endif

