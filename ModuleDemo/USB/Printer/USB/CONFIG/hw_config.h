#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "platform_config.h"
#include "usb_type.h"

#define USB_CLASS_DEVICE_PRINTR 1

#define USBD_VID 0x0519
#define USBD_PID 0x2013


#define USART_TX_DATA_SIZE 128  //USB print send FIFO size
#define USART_RX_DATA_SIZE 1024 //USB print buffer maximum bytes

//Define a USB USART FIFO structure
typedef struct
{
    u8 *buffer;   //buffer
    u16 writeptr; //write pointer
    u16 readptr;  //read pointer
} _usb_printer_fifo;

extern _usb_printer_fifo prt_txfifo; //USB serial port sends FIFO
extern _usb_printer_fifo prt_rxfifo; //USB serial port receives FIFO


static __inline int RING_IS_EMPTY(uint32_t head, uint32_t tail, uint32_t size)
{
    return head == tail;
}

static __inline int RING_IS_FULL(uint32_t head, uint32_t tail, uint32_t size)
{
    return tail == ((head + 1) % size);
}

static __inline int RING_CNT(uint32_t head, uint32_t tail, uint32_t size)
{
    // ((tail +size- head)&(size-1)) //size is 2^n
    //(head >= tail) ? (head - tail) : (size + head - tail); // Not support / cmd
    return ((head + size - tail) % (size));
}

static __inline int RING_SPACE(uint32_t head, uint32_t tail, uint32_t size)
{
    // ((head +size- tail)&(size-1))  // size is 2^n
    //(head >= tail) ? (size + tail - head - 1) :  (tail - head - 1);  // Not support / cmd
    return ((tail + size - head - 1) % (size)); //CIRC_CNT(tail, head+1, size);
}

//USB universal code function declaration
void USB_Port_Set(u8 enable);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config(FunctionalState NewState);
void IntToUnicode(u32 value, u8 *pbuf, u8 len);
void Get_SerialNum(void);

//Function declarations added by different USB functions
bool USB_Buffer_Config(void);
int  USB_Printer_RecvData(uint8_t *data_buffer, uint32_t Nb_bytes);
int  USB_Printer_WriterData(u8 *data, uint32_t Nb_bytes);
int  USB_Printer_GetRecvLen(void);


void usb_printf(char *fmt, ...);

#endif
