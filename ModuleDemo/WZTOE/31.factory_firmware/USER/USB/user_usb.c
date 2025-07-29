#include <stdio.h>
#include "user_usb.h"
#include "delay.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "string.h"

uint8_t  usbstatus         = 0;
uint8_t  usb_conn_flag     = 0;
uint8_t  usb_data[20][255] = {0};
uint16_t usb_data_len[20]  = {0};
uint8_t  data_cnt          = 0;
void     usb_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�ж����ȼ����� ��2��delay_ms(1800);
    delay_ms(1800);
    USB_Port_Set(0);                                //USB�ȶϿ�
    delay_ms(700);
    USB_Port_Set(1);                                //USB�ٴ�����
    Set_USBClock();
    USB_Interrupts_Config();
    USB_Init();
    DP_PUUP = 1;
}
void usb_run(void)
{
    uint16_t len;
    if (usbstatus != bDeviceState)
    {
        usbstatus = bDeviceState;
        if (usbstatus == CONFIGURED)
        {
            usb_conn_flag = 1;
            printf("USB connected\r\n");
        }
        else
        {
            usb_conn_flag = 0;
            printf("USB disconnected\r\n");
        }
    }
    if (USB_USART_RX_STA & 0x8000)
    {
        len = USB_USART_RX_STA & 0x3FFF;
        strncpy((char *)(usb_data[data_cnt]), (char *)USB_USART_RX_BUF, len);
        //sprintf((char*)(usb_data[data_cnt]),"%s",(char*)USB_USART_RX_BUF);
        usb_printf("Tx->:%s\r\n", usb_data[data_cnt]);
        USB_USART_RX_STA = 0;
        if (data_cnt < 18)
        {
            data_cnt++;
        }
        else
        {
            data_cnt = 0;
        }
    }
}
