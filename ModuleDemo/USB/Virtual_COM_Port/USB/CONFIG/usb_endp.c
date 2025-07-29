/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  NONE
  * @version NONE
  * @date    NONE
  * @brief   NONE
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
    u16        USB_Tx_ptr;
    static u16 USB_Tx_length = 0;

    if (uu_txfifo.readptr == uu_txfifo.writeptr) //No data to send, just exit
    {
        if (USB_Tx_length == VIRTUAL_COM_PORT_DATA_SIZE)
        {
            USB_Tx_length = 0;
            SetEPTxCount(ENDP1, USB_Tx_length);
            SetEPTxValid(ENDP1);
        }
        return;
    }
    if (uu_txfifo.readptr < uu_txfifo.writeptr)                    //No more than array, read pointer < write pointer
    {
        USB_Tx_length = uu_txfifo.writeptr - uu_txfifo.readptr;    //Get the length of data to be sent
    }
    else                                                           //Array exceeded, read pointer > write pointer
    {
        USB_Tx_length = USB_USART_TXFIFO_SIZE - uu_txfifo.readptr; //Get the length of data to be sent
    }
    if (USB_Tx_length > VIRTUAL_COM_PORT_DATA_SIZE)                //More than 64 bytes?
    {
        USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;                //The amount of data sent this time
    }
    USB_Tx_ptr         = uu_txfifo.readptr;                        //Send start address
    uu_txfifo.readptr += USB_Tx_length;                            //read pointer offset
    if (uu_txfifo.readptr >= USB_USART_TXFIFO_SIZE)                //Read pointer to zero
    {
        uu_txfifo.readptr = 0;
    }
    UserToPMABufferCopy(&uu_txfifo.buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
    SetEPTxCount(ENDP1, USB_Tx_length);
    SetEPTxValid(ENDP1);
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
    u16 USB_Rx_Cnt;
    USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer); //Get the data received by USB and its length
    USB_To_USART_Send_Data(USB_Rx_Buffer, USB_Rx_Cnt); //Processing data (actually, saving data)
    SetEPRxValid(ENDP3);                               //Data reception at endpoint 3
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
    static uint32_t FrameCount = 0;

    if (bDeviceState == CONFIGURED)
    {
        if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
        {
            /* Reset the frame counter */
            FrameCount = 0;

            /* Check the data to be sent through IN pipe */
            EP1_IN_Callback(); //EP1_IN_Callback function to send TX data to USB
                               //Handle_USBAsynchXfer();
        }
    }
}
/******************* (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/

