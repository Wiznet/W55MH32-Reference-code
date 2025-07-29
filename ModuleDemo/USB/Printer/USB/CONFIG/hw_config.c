#include "hw_config.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

uint8_t           USB_Printer_Rx_Buffer[USART_RX_DATA_SIZE];
uint8_t           USB_Printer_Tx_Buffer[USART_TX_DATA_SIZE];
_usb_printer_fifo prt_txfifo = {USB_Printer_Tx_Buffer, 0, 0}; //USB printer sends FIFO
_usb_printer_fifo prt_rxfifo = {USB_Printer_Rx_Buffer, 0, 0}; //USB printer receives FIFO

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

//USB clock configuration function, USBclk = 48Mhz@HCLK = 72Mhz
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
    //if (NewState!=DISABLE)printf("usb pull up enable\r\n");
    //else printf("usb pull up disable\r\n");
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
        Virtual_Com_Port_StringSerial[2] = (u8)(Device_Serial0 & 0x000000FF);
        Virtual_Com_Port_StringSerial[4] = (u8)((Device_Serial0 & 0x0000FF00) >> 8);
        Virtual_Com_Port_StringSerial[6] = (u8)((Device_Serial0 & 0x00FF0000) >> 16);
        Virtual_Com_Port_StringSerial[8] = (u8)((Device_Serial0 & 0xFF000000) >> 24);

        Virtual_Com_Port_StringSerial[10] = (u8)(Device_Serial1 & 0x000000FF);
        Virtual_Com_Port_StringSerial[12] = (u8)((Device_Serial1 & 0x0000FF00) >> 8);
        Virtual_Com_Port_StringSerial[14] = (u8)((Device_Serial1 & 0x00FF0000) >> 16);
        Virtual_Com_Port_StringSerial[16] = (u8)((Device_Serial1 & 0xFF000000) >> 24);

        Virtual_Com_Port_StringSerial[18] = (u8)(Device_Serial2 & 0x000000FF);
        Virtual_Com_Port_StringSerial[20] = (u8)((Device_Serial2 & 0x0000FF00) >> 8);
        Virtual_Com_Port_StringSerial[22] = (u8)((Device_Serial2 & 0x00FF0000) >> 16);
        Virtual_Com_Port_StringSerial[24] = (u8)((Device_Serial2 & 0xFF000000) >> 24);
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
bool USB_Buffer_Config(void)
{
    prt_txfifo.readptr  = 0; //Clear read pointer
    prt_txfifo.writeptr = 0; //Clear write pointer
    prt_txfifo.buffer   = USB_Printer_Tx_Buffer;

    memset(prt_txfifo.buffer, 0x00, sizeof(USART_TX_DATA_SIZE));

    prt_rxfifo.readptr  = 0; //Clear read pointer
    prt_rxfifo.writeptr = 0; //Clear write pointer
    prt_rxfifo.buffer   = USB_Printer_Rx_Buffer;
    memset(prt_rxfifo.buffer, 0x00, sizeof(USART_RX_DATA_SIZE));
    //printf("linecoding.format:%d\r\n", prt_rxfifo.readptr);
    //printf("linecoding.paritytype:%d\r\n",linecoding.paritytype);
    //printf("linecoding.datatype:%d\r\n",linecoding.datatype);
    //printf("linecoding.bitrate:%d\r\n",linecoding.bitrate);
    return (TRUE);
}

//Processing data received from USB printing
//Databuffer: data buffer
//Nb_bytes: Number of bytes received.
int USB_Printer_RecvData(u8 *data_buffer, u32 Nb_bytes)
{
    int i, recv_byte = 0;
    recv_byte = RING_CNT(prt_rxfifo.writeptr, prt_rxfifo.readptr, USART_RX_DATA_SIZE);
    recv_byte = recv_byte > Nb_bytes ? Nb_bytes : recv_byte;

    for (i = 0; i < recv_byte; i++)
    {
        data_buffer[i] = prt_rxfifo.buffer[prt_rxfifo.readptr++];
        if (prt_rxfifo.readptr == USART_RX_DATA_SIZE) prt_rxfifo.readptr = 0;
    }

    //memcpy(data_buffer, (u8 *)&prt_rxfifo.buffer[prt_rxfifo.readptr], recv_byte);
    //prt_rxfifo.readptr = (prt_rxfifo.readptr + recv_byte)%USART_RX_DATA_SIZE;

    return recv_byte;
}

int USB_Printer_GetRecvLen(void)
{
    int recv_byte = 0;
    recv_byte     = RING_CNT(prt_rxfifo.writeptr, prt_rxfifo.readptr, USART_RX_DATA_SIZE);
    return recv_byte;
}

//Send data to USB for printing
int USB_Printer_WriterData(u8 *data, u32 Nb_bytes)
{
    int w_byte = 0; //, pack_num = 0;
    int i      = 0;

    if (bDeviceState != CONFIGURED) return 0;


    w_byte = RING_SPACE(prt_txfifo.writeptr, prt_txfifo.readptr, USART_TX_DATA_SIZE);
    if (w_byte == 0) return 0;

    w_byte = Nb_bytes > w_byte ? w_byte : Nb_bytes;
    for (i = 0; i < w_byte; i++)
    {
        prt_txfifo.buffer[prt_txfifo.writeptr++] = data[i];
        if (prt_txfifo.writeptr == USART_TX_DATA_SIZE) prt_txfifo.writeptr = 0;
    }
    //memcpy((u8 *)prt_txfifo.buffer[prt_txfifo.writeptr], (u8 *)data, w_byte);
    //prt_txfifo.writeptr = (prt_txfifo.writeptr + w_byte)%USART_TX_DATA_SIZE;

    return w_byte;
}

#if 0
char USART_PRINTF_Buffer[512];
//USB virtual serial port, printf function
//Ensure that the data sent does not exceed USB_USART_REC_LEN bytes
void usb_printf(char* fmt,...)  
{  
	u16 i;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_PRINTF_Buffer);//The length of the data sent this time
	USB_Printer_WriterData((u8 *)USART_PRINTF_Buffer, i); 

}
#endif
