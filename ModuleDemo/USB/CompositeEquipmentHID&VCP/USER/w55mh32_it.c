/**
  ******************************************************************************
  * @file    GPIO/IOToggle/w55mh32_it.c 
  * @author  NONE
  * @version NONE
  * @date    NONE
  * @brief   NONE
  ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "w55mh32_it.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "platform_config.h"
__IO uint8_t         Send_Buffer[2];
extern __IO uint8_t  PrevXferComplete;
extern uint32_t      ADC_ConvertedValueX;
extern uint32_t      ADC_ConvertedValueX_1;
extern __IO uint32_t TimingDelay;
void                 USBWakeUp_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line18);
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}
/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    Send_Buffer[0] = 0x07;

    if ((ADC_ConvertedValueX >> 4) - (ADC_ConvertedValueX_1 >> 4) > 4)
    {
        if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
        {
            Send_Buffer[1] = (uint8_t)(ADC_ConvertedValueX >> 4);

            /* Write the descriptor through the endpoint */
            USB_SIL_Write(EP1_IN, (uint8_t *)Send_Buffer, 2);
            SetEPTxValid(ENDP1);
            ADC_ConvertedValueX_1 = ADC_ConvertedValueX;
            PrevXferComplete      = 0;
        }
    }

    DMA_ClearFlag(DMA1_FLAG_TC1);
}

/*******************************************************************************
* Function Name  : EXTI_IRQHandler
* Description    : This function handles External lines interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    uint32_t i = 0x500;
    while (i--);
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
        {
            Send_Buffer[0] = 0x05;
            if (WIZ_EVAL_PBGetState(KEY1) == Bit_RESET)
            {
                Send_Buffer[1] = 0x01;
            }
            else
            {
                Send_Buffer[1] = 0x00;
            }

            /* Write the descriptor through the endpoint */
            USB_SIL_Write(EP1_IN, (uint8_t *)Send_Buffer, 2);
            SetEPTxValid(ENDP1);
            PrevXferComplete = 0;
        }
        /* Clear the EXTI line  pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
        {
            Send_Buffer[0] = 0x06;

            if (WIZ_EVAL_PBGetState(KEY2) == Bit_RESET)
            {
                Send_Buffer[1] = 0x01;
            }
            else
            {
                Send_Buffer[1] = 0x00;
            }

            /* Write the descriptor through the endpoint */
            USB_SIL_Write(EP1_IN, (uint8_t *)Send_Buffer, 2);

            SetEPTxValid(ENDP1);

            PrevXferComplete = 0;
        }
        /* Clear the EXTI line 13 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}


void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    while (1);
}

/******************************************************************************/
/*                 w55mh32 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_w55mh32_xx.s).                                            */
/******************************************************************************/
