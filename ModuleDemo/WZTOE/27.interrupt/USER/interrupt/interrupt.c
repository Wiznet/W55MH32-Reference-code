#include "interrupt.h"
#include "socket.h" // Use socket
#include "wiz_interface.h"
#include <stdio.h>
#include "string.h"
#include "w55mh32.h"

#define ETHERNET_BUF_MAX_SIZE (1024 * 2) // Send and receive cache size
#define INTERRUPT_DEBUG
#define IR_SOCK(ch) (0x01 << ch)         /**< check socket interrupt */

enum SN_STATUS
{
    closed_status = 0,
    ready_status,
    connected_status,
};
static uint8_t I_STATUS[_WIZCHIP_SOCK_NUM_];
static uint8_t ch_status[_WIZCHIP_SOCK_NUM_] = {0};

void wizchip_int_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource8);

    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    EXTI_InitStructure.EXTI_Line    = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line8) == SET)
    {
        if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8) == RESET)
        {
            wizchip_ISR();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line8);
}


/**
 * @brief   Determine the interrupt type and store the value in I STATUS
 * @param   none
 * @return  none
 */
void wizchip_ISR(void)
{
    uint8_t SIR_val = 0;
    uint8_t tmp, sn;
    SIR_val = getSIR();
    if (SIR_val != 0xff)
    {
        setSIMR(0x00);
        for (sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++)
        {
            tmp = 0;
            if (SIR_val & IR_SOCK(sn))
            {
                tmp           = getSn_IR(sn);
                I_STATUS[sn] |= tmp;
                tmp          &= 0x0f;
                setSn_IR(sn, tmp);
            }
        }
        setSIMR(0xff);
    }
}

/**
 * @brief   TCP server interrupt mode loopback
 * @param   none
 * @return  none
 */
void loopback_tcps_interrupt(uint8_t sn, uint8_t *buf, uint16_t port)
{
    uint16_t len = 0;
    uint8_t  destip[4];
    uint16_t destport;

    if (I_STATUS[sn] == SOCK_CLOSED)
    {
        if (!ch_status[sn])
        {
#ifdef INTERRUPT_DEBUG
            printf("%d:TCP server start\r\n", sn);
#endif
            ch_status[sn] = ready_status;

            if (socket(sn, Sn_MR_TCP, port, 0x00) != sn)
            {
                ch_status[sn] = closed_status;
            }
            else
            {
#ifdef INTERRUPT_DEBUG
                printf("%d:Socket opened\r\n", sn);
#endif
                listen(sn);
#ifdef INTERRUPT_DEBUG
                printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
            }
        }
    }
    if (I_STATUS[sn] & Sn_IR_CON)
    {
        getSn_DIPR(sn, destip);
        destport = getSn_DPORT(sn);
#ifdef INTERRUPT_DEBUG
        printf("%d:Connected - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);

#endif
        ch_status[sn]  = connected_status;
        I_STATUS[sn]  &= ~(Sn_IR_CON);
    }

    if (I_STATUS[sn] & Sn_IR_DISCON)
    {
        printf("%d:Socket disconnected\r\n", sn);
        if ((getSn_RX_RSR(sn)) > 0)
        {
            if (len > ETHERNET_BUF_MAX_SIZE)
            {
                len = ETHERNET_BUF_MAX_SIZE;
            }
            recv(sn, buf, len);
            buf[len] = 0x00;
            printf("%d:recv data:%s\r\n", sn, buf);
            I_STATUS[sn] &= ~(Sn_IR_RECV);
            send(sn, buf, len);
        }
        disconnect(sn);
        ch_status[sn]  = closed_status;
        I_STATUS[sn]  &= ~(Sn_IR_DISCON);
    }

    if (I_STATUS[sn] & Sn_IR_RECV)
    {
        setIMR(0x00);
        I_STATUS[sn] &= ~(Sn_IR_RECV);
        setIMR(0xff);
        if ((len = getSn_RX_RSR(sn)) > 0)
        {
            if (len > ETHERNET_BUF_MAX_SIZE)
            {
                len = ETHERNET_BUF_MAX_SIZE;
            }
            len      = recv(sn, buf, len);
            buf[len] = 0x00;
            printf("%d:recv data:%s\r\n", sn, buf);
            send(sn, buf, len);
        }
    }

    if (I_STATUS[sn] & Sn_IR_SENDOK)
    {
        I_STATUS[sn] &= ~(Sn_IR_SENDOK);
    }
}
