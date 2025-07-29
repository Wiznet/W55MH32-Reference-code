/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  none
  * @version V4.1.0
  * @date    17-Oct-2024
  * @brief   Endpoint routines
  ******************************************************************************
*/

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
typedef struct ep_buf
{
    uint8_t aucBuf[0x40];
    uint8_t ucLen;
} EP_Buf_t;
EP_Buf_t EP_Buf[8] = {0};
uint16_t EP_IN_ADDR[8] =
    {
        0x80,
        0xC0,
        0x100,
        0x140,
        0x180,
        0x1A0,
        0x1C0,
        0x1E0,
};
uint16_t EP_OUT_ADDR[8] =
    {
        0x40,
        0xE0,
        0x120,
        0x160,
        0x190,
        0x1B0,
        0x1D0,
        0x1F0,
};

void EP_IN_Process(uint8_t EP)
{
    SetEPRxValid(EP);
}

void EP_OUT_Process(uint8_t EP)
{
    EP_Buf[EP].ucLen = GetEPRxCount(EP);
    PMAToUserBufferCopy(EP_Buf[EP].aucBuf, EP_OUT_ADDR[EP], EP_Buf[EP].ucLen);

    printf("EP:0x%X Received data: ", EP);
    for (int t = 0; t < EP_Buf[EP].ucLen; t++)
    {
        printf("%c", EP_Buf[EP].aucBuf[t]); //In bytes, sent to USB
    }
    printf("\n");

    UserToPMABufferCopy(EP_Buf[EP].aucBuf, EP_IN_ADDR[EP], EP_Buf[EP].ucLen);
    SetEPTxCount(EP, EP_Buf[EP].ucLen);
    SetEPTxValid(EP);
}

void EP1_IN_Callback(void)
{
    EP_IN_Process(ENDP1);
}

void EP1_OUT_Callback(void)
{
    EP_OUT_Process(ENDP1);
}

void EP2_IN_Callback(void)
{
    EP_IN_Process(ENDP2);
}

void EP2_OUT_Callback(void)
{
    EP_OUT_Process(ENDP2);
}

void EP3_IN_Callback(void)
{
    EP_IN_Process(ENDP3);
}

void EP3_OUT_Callback(void)
{
    EP_OUT_Process(ENDP3);
}

void EP4_IN_Callback(void)
{
    EP_IN_Process(ENDP4);
}

void EP4_OUT_Callback(void)
{
    EP_OUT_Process(ENDP4);
}

void EP5_IN_Callback(void)
{
    EP_IN_Process(ENDP5);
}

void EP5_OUT_Callback(void)
{
    EP_OUT_Process(ENDP5);
}

void EP6_IN_Callback(void)
{
    EP_IN_Process(ENDP6);
}

void EP6_OUT_Callback(void)
{
    EP_OUT_Process(ENDP6);
}

void EP7_IN_Callback(void)
{
    EP_IN_Process(ENDP7);
}

void EP7_OUT_Callback(void)
{
    EP_OUT_Process(ENDP7);
}

