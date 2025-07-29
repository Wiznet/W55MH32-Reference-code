/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  none
  * @version V4.1.0
  * @date    17-Oct-2024
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; copyright (c) 2024 WIZnet. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of WIZnet nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for WIZnet.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY WIZnet AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL WIZnet OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_bot.h"
#include "usb_istr.h"
#include "usb_conf.h"
#include "cdc_usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    : EP1 IN Callback Routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
    Mass_Storage_In();
}

/*******************************************************************************
* Function Name  : EP2_OUT_Callback.
* Description    : EP2 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
    Mass_Storage_Out();
}

void CDC_Send_Data(CDC_USART_t *usart, uint8_t is_in_callback)
{
    uint8_t data_size;
    uint8_t aucBuf[usart->in_endp.max_data_size];

    data_size = Buf_Get_Used_Size(&usart->in_buf);
    if (is_in_callback)
    {
        usart->in_busy = 0;
        if (data_size == 0 && usart->in_len == 0)
            return;
    }
    else
    {
        if (usart->in_busy || data_size == 0)
            return;
    }

    if (data_size > usart->in_endp.max_data_size)
        usart->in_len = usart->in_endp.max_data_size;
    else
        usart->in_len = data_size;
    if (usart->in_len)
    {
        Buf_Read_Bytes(&usart->in_buf, aucBuf, usart->in_len);
        UserToPMABufferCopy(aucBuf, usart->in_endp.data_addr, usart->in_len);
    }
    SetEPTxCount(usart->in_endp.ENDP, usart->in_len);
    SetEPTxValid(usart->in_endp.ENDP);
    usart->in_busy = 1;
}

void CDC_OUT_Callback(CDC_USART_t *usart)
{
    usart->out_len = GetEPRxCount(usart->out_endp.ENDP);
    if (usart->out_len == 0)
        return;
    PMAToUserBufferCopy(usart->out_buf, usart->out_endp.data_addr, usart->out_len);
    usart->out_pos = 0;
    USART_ITConfig(usart->USARTx, USART_IT_TXE, ENABLE);
}

void EP3_IN_Callback(void)
{
    CDC_Send_Data(&CDC_USART[CDC_USART1], 1);
}

void EP3_OUT_Callback(void)
{
    CDC_OUT_Callback(&CDC_USART[CDC_USART1]);
}

void EP4_IN_Callback(void)
{
    CDC_Send_Data(&CDC_USART[CDC_USART2], 1);
}

void EP4_OUT_Callback(void)
{
    CDC_OUT_Callback(&CDC_USART[CDC_USART2]);
}

void SOF_Callback(void)
{
    static uint8_t Sof_Count = 0;
    Sof_Count++;
    if (Sof_Count == 5)
    {
        Sof_Count = 0;
        CDC_Send_Data(&CDC_USART[CDC_USART1], 0);
        CDC_Send_Data(&CDC_USART[CDC_USART2], 0);
    }
}

/************************ (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/

