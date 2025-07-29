#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "platform_config.h"
#include "usb_type.h"


#define USB_USART_TXFIFO_SIZE 1024 //USB virtual serial port sending FIFO size
#define USB_USART_REC_LEN     200  //USB serial port receive buffer maximum number of bytes

//Define a USB USART FIFO structure
typedef struct
{
    u8   buffer[USB_USART_TXFIFO_SIZE];         //buffer
    vu16 writeptr;                              //write pointer
    vu16 readptr;                               //read pointer
} _usb_usart_fifo;
extern _usb_usart_fifo uu_txfifo;               //USB serial port sends FIFO

extern u8  USB_USART_RX_BUF[USB_USART_REC_LEN]; //Receive buffer, maximum USB_USART_REC_LEN bytes. Last byte is newline
extern u16 USB_USART_RX_STA;                    //receive status flag

//USB universal code function declaration
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config(FunctionalState NewState);
void USB_Port_Set(u8 enable);
void IntToUnicode(u32 value, u8 *pbuf, u8 len);
void Get_SerialNum(void);

//Function declarations added by different USB functions
bool USART_Config(void);
void USB_To_USART_Send_Data(uint8_t *data_buffer, uint8_t Nb_bytes);
void USART_To_USB_Send_Data(void);
void USB_USART_SendData(u8 data);
void usb_printf(char *fmt, ...);

#endif
