#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "delay.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "wiz.h"

USART_TypeDef    *USART_TEST = USART1;
void              UART_Configuration(uint32_t bound);
void              RCC_ClkConfiguration(void);
__IO uint8_t      PrevXferComplete = 1;
RCC_ClocksTypeDef clocks;
__IO uint32_t     TimingDelay = 0;

int main(void)
{
    uint8_t usbstatus = 0;

    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("Composite Equipment HID&VCP\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    Set_System();
    Set_USBClock();
    USB_Interrupts_Config();
    DP_PUUP = 1;
    USB_Init();

    while (1)
    {
        if (bDeviceState == CONFIGURED)
        {
            /*Check to see if we have data yet */
            if (Receive_length_port != 0 && packet_sent == 1)
            {
                CDC_Send_DATA((unsigned char *)Receive_Buffer_port, Receive_length_port);
                Receive_length_port = 0;
                SetEPRxValid(ENDP7);
            }
        }
        if (usbstatus != bDeviceState) //The USB connection status has changed.
        {
            usbstatus = bDeviceState;  //Record the new status.
            if (usbstatus == CONFIGURED)
            {
                printf("USB connection successful\n");
            }
            else
            {
                printf("USB disconnected\n");
            }
        }
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

