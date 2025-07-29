/**
  ******************************************************************************
  * @file    mass_mal.h
  * @author  none
  * @version V4.1.0
  * @date    17-Oct-2024
  * @brief   Header for mass_mal.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 WIZnet</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of WIZnet nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASS_MAL_H
#define __MASS_MAL_H

/* Includes ------------------------------------------------------------------*/
#include "string.h"
/* Exported types ------------------------------------------------------------*/

typedef struct
{
    uint16_t (*Init)(void);
    uint16_t (*Write)(uint32_t Memory_Offset, uint8_t *Writebuff, uint16_t Transfer_Length);
    uint16_t (*Read)(uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length);
    uint16_t (*GetStatus)(uint8_t lun);
    uint8_t *Inquiry_Data;
} t_Mal_Lun;


/* Exported constants --------------------------------------------------------*/
#define MAL_OK      0
#define MAL_FAIL    1
#define MAX_LUN_NUM 1

/* Exported macro ------------------------------------------------------------*/
extern uint32_t Mass_Block_Size[MAX_LUN_NUM];
extern uint32_t Mass_Block_Count[MAX_LUN_NUM];
/* Exported functions ------------------------------------------------------- */

uint16_t MAL_Init(uint8_t lun);
uint16_t MAL_GetStatus(uint8_t lun);
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length);
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length);
uint8_t *MAL_Standard_Inquiry_Data(uint8_t lun);
#endif /* __MASS_MAL_H */

/************************ (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/
