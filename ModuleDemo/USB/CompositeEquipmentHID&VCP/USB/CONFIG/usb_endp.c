

/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "usb_lib.h"
#include "stdio.h"
#include "usb_istr.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[2];

extern __IO uint8_t PrevXferComplete;
uint8_t             Receive_Buffer_port[64];
uint32_t            Receive_length_port;
int                 packet_sent = 0;
uint32_t            CDC_Send_DATA(uint8_t *ptrBuffer, uint8_t Send_length);

uint32_t CDC_Send_DATA(uint8_t *ptrBuffer, uint8_t Send_length)
{
    /*if max buffer is Not reached*/
    if (Send_length <= VIRTUAL_COM_PORT_DATA_SIZE)
    {
        /*Sent flag*/
        packet_sent = 0;
        /* send  packet to PMA*/
        UserToPMABufferCopy((unsigned char *)ptrBuffer, ENDP7_TXADDR, Send_length);
        SetEPTxCount(ENDP7, Send_length);
        //	 delay_us(100);
        SetEPTxValid(ENDP7);
    }
    else
    {
        return 0;
    }
    return 1;
}

void EP1_IN_Callback(void)
{
    packet_sent = 1;
}
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
    /* Read received data (2 bytes) */
    USB_SIL_Read(EP1_OUT, Receive_Buffer);

    printf("HID Received data: ");
    for (int t = 0; t < 2; t++)
    {
        printf("0x%02X ", Receive_Buffer[t]); //In bytes, sent to USB
    }
    printf("\n");
    SetEPRxStatus(ENDP1, EP_RX_VALID);
}


void EP7_OUT_Callback(void)
{
    Receive_length_port = GetEPRxCount(ENDP7);
    PMAToUserBufferCopy((unsigned char *)Receive_Buffer_port, ENDP7_RXADDR, Receive_length_port);
    printf("VCP Received data: ");
    for (int t = 0; t < Receive_length_port; t++)
    {
        printf("%c", Receive_Buffer_port[t]); //In bytes, sent to USB
    }
    printf("\n");
}

void EP7_IN_Callback(void)
{
    packet_sent = 1;
}
