/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  NONE
  * @version NONE
  * @date    NONE
  * @brief   NONE
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "dfu_mal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : FLASH_If_Init
* Description    : Initializes the Media on the wiz
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Init(void)
{
    FLASH_Unlock();
    return MAL_OK;
}

/*******************************************************************************
* Function Name  : FLASH_If_Erase
* Description    : Erase sector
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Erase(uint32_t SectorAddress)
{
    FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP);
    FLASH_ErasePage(SectorAddress);
    return MAL_OK;
}

/*******************************************************************************
* Function Name  : FLASH_If_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t FLASH_If_Write(uint32_t SectorAddress, uint32_t DataLength)
{
    uint32_t idx = 0;
    if (DataLength & 0x3) /* Not an aligned data */
    {
        for (idx = DataLength; idx < ((DataLength & 0xFFFC) + 4); idx++)
        {
            MAL_Buffer[idx] = 0xFF;
        }
    }

    /* Data received are Word multiple */
    for (idx = 0; idx < DataLength; idx = idx + 4)
    {
        FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP);
        FLASH_ProgramWord(SectorAddress, *(uint32_t *)(MAL_Buffer + idx));
        SectorAddress += 4;
    }

    return MAL_OK;
}

/*******************************************************************************
* Function Name  : FLASH_If_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : buffer address pointer
*******************************************************************************/
uint8_t *FLASH_If_Read(uint32_t SectorAddress, uint32_t DataLength)
{
    return (uint8_t *)(SectorAddress);
}

/******************* (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/
