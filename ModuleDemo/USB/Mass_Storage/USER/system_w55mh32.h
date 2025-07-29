/**
  ******************************************************************************
  * @file    system_w55mh32.h
  * @author  none
  * @version V3.5.0
  * @date    17-Oct-2024
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Header File.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2024 WIZnet</center></h2>
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup w55mh32_system
  * @{
  */

/**
  * @brief Define to prevent recursive inclusion
  */
#ifndef __SYSTEM_W55MH32_H
#define __SYSTEM_W55MH32_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup w55mh32_System_Includes
  * @{
  */

/**
  * @}
  */


/** @addtogroup w55mh32_System_Exported_types
  * @{
  */

extern uint32_t SystemCoreClock; /*!< System Clock Frequency (Core Clock) */

/**
  * @}
  */

/** @addtogroup w55mh32_System_Exported_Constants
  * @{
  */

/**
  * @}
  */

/** @addtogroup w55mh32_System_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup w55mh32_System_Exported_Functions
  * @{
  */

extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_w55mh32_H */

/**
  * @}
  */

/**
  * @}
  */
/******************* (C) COPYRIGHT 2024 WIZnet *****END OF FILE****/
