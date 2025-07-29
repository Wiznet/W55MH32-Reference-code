/**
  ******************************************************************************
  * @file    GPIO/IOToggle/w55mh32_it.h 
  * @author  none
  * @version V3.5.0
  * @date    
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W55MH32_IT_H
#define __W55MH32_IT_H

/* Includes ------------------------------------------------------------------*/
#include "w55mh32.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif /* __w55mh32_IT_H */

