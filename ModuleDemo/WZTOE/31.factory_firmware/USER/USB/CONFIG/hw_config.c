#include "hw_config.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

_usb_usart_fifo uu_txfifo;                              //USB serial port sends FIFO structure
u8              USART_PRINTF_Buffer[USB_USART_REC_LEN]; //usb_printf Send Buffer

//Use a method similar to serial port 1 to receive data to process the data received by the USB virtual serial port.
u8 USB_USART_RX_BUF[USB_USART_REC_LEN]; //Receive buffer, maximum USART_REC_LEN bytes.
//receiving status
//bit15, receive the completion flag.
//bit14, received 0x0d
//bit13~ 0, the number of valid bytes received
u16 USB_USART_RX_STA = 0;      //receive status flag

extern LINE_CODING linecoding; //USB virtual serial port configuration information
//USB Wake Interrupt Service Function
void USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18); //Clear USB Wake Interrupt Pause Bit
}

//USB interrupt handler
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}

//USB clock configuration function,USBclk=48Mhz@HCLK=72Mhz
void Set_USBClock(void)
{
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_4Div5);    //USBclk=PLLclk/1.5=48Mhz
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE); //USB clock enabled
}

//USB enters low power mode
//When USB enters suspend mode, MCU enters low power mode
//You need to add low-power code by yourself (such as turning off the clock, etc.)
void Enter_LowPowerMode(void)
{
    printf("usb enter low power mode\r\n");
    bDeviceState = SUSPENDED;
}

//USB exits low power mode
//Users can add relevant code by themselves (such as reconfiguring the clock, etc.)
void Leave_LowPowerMode(void)
{
    DEVICE_INFO *pInfo = &Device_Info;
    printf("leave low power mode\r\n");
    if (pInfo->Current_Configuration != 0)
        bDeviceState = CONFIGURED;
    else
        bDeviceState = ATTACHED;
}

//USB interrupt configuration
void USB_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Configure the EXTI line 18 connected internally to the USB IP */
    EXTI_ClearITPendingBit(EXTI_Line18);
    //  Open interrupt on line 18
    EXTI_InitStructure.EXTI_Line    = EXTI_Line18;         // USB resume from suspend mode
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //line 18 on event lift edge triggered
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the USB interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn; //Group 2, second priority
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB Wake-up interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USBWakeUp_IRQn; //Group 2, highest priority
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

//USB interface configuration
//NewState: DISABLE, don't pull
//ENABLE, pull up
void USB_Cable_Config(FunctionalState NewState)
{
    if (NewState != DISABLE)
        printf("usb pull up enable\r\n");
    else
        printf("usb pull up disable\r\n");
}

//USB enable connect/disconnect
//enable: 0, disconnect
//1, allow connection
void USB_Port_Set(u8 enable)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //Enable PORTA clock
    if (enable)
        _SetCNTR(_GetCNTR() & (~(1 << 1)));               //Exit power down mode
    else
    {
        _SetCNTR(_GetCNTR() | (1 << 1)); // power down mode
        GPIOA->CRH &= 0XFFF00FFF;
        GPIOA->CRH |= 0X00033000;
        PAout(12)   = 0;
    }
}

//For USB configuration information
void Get_SerialNum(void)
{
    u32 Device_Serial0, Device_Serial1, Device_Serial2;
    Device_Serial0  = *(u32 *)(0x1FFFF7E8);
    Device_Serial1  = *(u32 *)(0x1FFFF7EC);
    Device_Serial2  = *(u32 *)(0x1FFFF7F0);
    Device_Serial0 += Device_Serial2;
    if (Device_Serial0 != 0)
    {
        IntToUnicode(Device_Serial0, &Virtual_Com_Port_StringSerial[2], 8);
        IntToUnicode(Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
    }
}

//Converts the 32-bit value to unicode.
//value, the value to be converted (32bit)
//pbuf: store address
//len: length to convert
void IntToUnicode(u32 value, u8 *pbuf, u8 len)
{
    u8 idx = 0;
    for (idx = 0; idx < len; idx++)
    {
        if (((value >> 28)) < 0xA)
        {
            pbuf[2 * idx] = (value >> 28) + '0';
        }
        else
        {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value             = value << 4;
        pbuf[2 * idx + 1] = 0;
    }
}
/////////////////////////////////////////////////////////////////////////////////

//The configuration information of the USB COM port is printed through this function.
bool USART_Config(void)
{
    uu_txfifo.readptr  = 0; //Clear read pointer
    uu_txfifo.writeptr = 0; //Clear write pointer
    USB_USART_RX_STA   = 0; //USB USART reception status cleared
    printf("linecoding.format:%d\r\n", linecoding.format);
    printf("linecoding.paritytype:%d\r\n", linecoding.paritytype);
    printf("linecoding.datatype:%d\r\n", linecoding.datatype);
    printf("linecoding.bitrate:%d\r\n", linecoding.bitrate);
    return (TRUE);
}

//Processing the data received from the USB virtual serial port
//Databuffer: data buffer
//Nb_bytes: Number of bytes received.
void USB_To_USART_Send_Data(u8 *data_buffer, u8 Nb_bytes)
{
    u8 i;
    u8 res;
    for (i = 0; i < Nb_bytes; i++)
    {
        res = data_buffer[i];
        if ((USB_USART_RX_STA & 0x8000) == 0) //Receive not completed
        {
            if (USB_USART_RX_STA & 0x4000)    //Received 0x0d.
            {
                if (res != 0x0a)
                    USB_USART_RX_STA = 0;       //Error received, restart
                else
                    USB_USART_RX_STA |= 0x8000; //Reception complete
            }
            else                                //Haven't received 0X0D yet.
            {
                if (res == 0x0d)
                    USB_USART_RX_STA |= 0x4000;
                else
                {
                    USB_USART_RX_BUF[USB_USART_RX_STA & 0X3FFF] = res;
                    USB_USART_RX_STA++;
                    if (USB_USART_RX_STA > (USB_USART_REC_LEN - 1)) USB_USART_RX_STA = 0; //Error receiving data, start receiving again
                }
            }
        }
    }
}

//Send a byte of data to a USB virtual serial port
void USB_USART_SendData(u8 data)
{
    uu_txfifo.buffer[uu_txfifo.writeptr] = data;
    uu_txfifo.writeptr++;
    if (uu_txfifo.writeptr == USB_USART_TXFIFO_SIZE) //It exceeds the size of buf and returns to zero.
    {
        uu_txfifo.writeptr = 0;
    }
}

//usb virtual serial port, printf function
//Ensure that the data sent at one time does not exceed USB_USART_REC_LEN bytes
void usb_printf(char *fmt, ...)
{
    u16     i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART_PRINTF_Buffer, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART_PRINTF_Buffer); //The length of the data sent this time
    for (j = 0; j < i; j++)                        //loop data
    {
        USB_USART_SendData(USART_PRINTF_Buffer[j]);
    }
}
