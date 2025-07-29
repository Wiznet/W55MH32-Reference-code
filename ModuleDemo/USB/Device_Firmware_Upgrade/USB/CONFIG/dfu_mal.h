/**
  ******************************************************************************
  * @file    dfu_mal.h
  * @author  NONE
  * @version NONE
  * @date    NONE
  * @brief   NONE
  ******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DFU_MAL_H
#define __DFU_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "dfu_mal.h"
#include "usb_desc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define MAL_OK         0
#define MAL_FAIL       1
#define MAX_USED_MEDIA 3
#define MAL_MASK       0xFF000000

#define INTERNAL_FLASH_BASE 0x08000000
#define INTERNAL_OPT_BASE   0x1F000000

/* utils macro ---------------------------------------------------------------*/
#define _1st_BYTE(x) (uint8_t)((x) & 0xFF)               /* 1st addressing cycle */
#define _2nd_BYTE(x) (uint8_t)(((x) & 0xFF00) >> 8)      /* 2nd addressing cycle */
#define _3rd_BYTE(x) (uint8_t)(((x) & 0xFF0000) >> 16)   /* 3rd addressing cycle */
#define _4th_BYTE(x) (uint8_t)(((x) & 0xFF000000) >> 24) /* 4th addressing cycle */
/* Exported macro ------------------------------------------------------------*/
#define SET_POLLING_TIMING(x) \
    buffer[1] = _1st_BYTE(x); \
    buffer[2] = _2nd_BYTE(x); \
    buffer[3] = _3rd_BYTE(x);

/* Exported functions ------------------------------------------------------- */

uint16_t MAL_Init(void);
uint16_t MAL_Erase(uint32_t SectorAddress);
uint16_t MAL_Write(uint32_t SectorAddress, uint32_t DataLength);
uint8_t *MAL_Read(uint32_t SectorAddress, uint32_t DataLength);
uint16_t MAL_GetStatus(uint32_t SectorAddress, uint8_t Cmd, uint8_t *buffer);

extern uint8_t MAL_Buffer[wTransferSize]; /* RAM Buffer for Downloaded Data */
#endif                                    /* __DFU_MAL_H */

/******************* (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/
