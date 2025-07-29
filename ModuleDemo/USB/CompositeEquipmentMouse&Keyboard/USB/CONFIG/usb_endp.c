/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  NONE
  * @version NONE
  * @date    NONE
  * @brief   NONE
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t PrevXferComplete  = 1;
__IO uint8_t PrevXferComplete1 = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
    /* Set the transfer complete token to inform upper layer that the current 
  transfer has been complete */
    PrevXferComplete = 1;
}

/**
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_IN_Callback(void)
{
    /* Set the transfer complete token to inform upper layer that the current 
  transfer has been complete */
    PrevXferComplete1 = 1;
}

/******************* (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/

