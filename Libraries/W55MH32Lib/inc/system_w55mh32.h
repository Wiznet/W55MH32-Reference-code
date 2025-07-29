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

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */

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

#endif /*__SYSTEM_W55MH32_H */

/**
  * @}
  */
  
/**
  * @}
  */  
/******************* (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/
